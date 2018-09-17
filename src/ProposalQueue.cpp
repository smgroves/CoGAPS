#include "ProposalQueue.h"
#include "math/Random.h"
#include "utils/GapsAssert.h"

//////////////////////////////// AtomicProposal ////////////////////////////////

AtomicProposal::AtomicProposal(char t)
    : pos(0), atom1(NULL), atom2(NULL), type(t)
{}
    
//////////////////////////////// ProposalQueue /////////////////////////////////

ProposalQueue::ProposalQueue(unsigned nrow, unsigned ncol)
    :
mUsedMatrixIndices(nrow),
mMinAtoms(0),
mMaxAtoms(0),
mBinLength(std::numeric_limits<uint64_t>::max() / static_cast<uint64_t>(nrow * ncol)),
mNumCols(ncol),
mAlpha(0.0),
mDomainLength(static_cast<double>(mBinLength * static_cast<uint64_t>(nrow * ncol))),
mNumBins(static_cast<double>(nrow * ncol)),
mU1(0.f),
mU2(0.f),
mUseCachedRng(false)
{}

void ProposalQueue::setAlpha(float alpha)
{
    mAlpha = static_cast<double>(alpha);
}

void ProposalQueue::populate(AtomicDomain &domain, unsigned limit)
{
    GAPS_ASSERT(mQueue.empty());
    GAPS_ASSERT(mUsedAtoms.isEmpty());
    GAPS_ASSERT(mUsedMatrixIndices.isEmpty());
    GAPS_ASSERT(mMinAtoms == mMaxAtoms);
    GAPS_ASSERT(mMaxAtoms == domain.size());

    unsigned nIter = 0;
    bool success = true;
    while (nIter++ < limit && success)
    {
        if (!makeProposal(domain))
        {
            success = false;
            mUseCachedRng = true;
        }
        else
        {
            //gaps_printf("%c %llu\n", mQueue.back().type, mQueue.back().atom1->pos);
        }
    }
}

void ProposalQueue::clear()
{
    GAPS_ASSERT(mMinAtoms == mMaxAtoms);

    mQueue.clear();
    mUsedMatrixIndices.clear();
    mUsedAtoms.clear();
}

unsigned ProposalQueue::size() const
{
    return mQueue.size();
}

AtomicProposal& ProposalQueue::operator[](int n)
{
    GAPS_ASSERT(mQueue.size() > 0);
    GAPS_ASSERT(n < mQueue.size());

    return mQueue[n];
}

void ProposalQueue::acceptDeath()
{
    #pragma omp atomic
    --mMaxAtoms;
}

void ProposalQueue::rejectDeath()
{
    #pragma omp atomic
    ++mMinAtoms;
}

void ProposalQueue::acceptBirth()
{
    #pragma omp atomic
    ++mMinAtoms;
}

void ProposalQueue::rejectBirth()
{
    #pragma omp atomic
    --mMaxAtoms;
}

float ProposalQueue::deathProb(double nAtoms) const
{
    double numer = nAtoms * mDomainLength;
    return numer / (numer + mAlpha * mNumBins * (mDomainLength - nAtoms));
}

bool ProposalQueue::makeProposal(AtomicDomain &domain)
{
    mU1 = mUseCachedRng ? mU1 : mRng.uniform();
    mU2 = mUseCachedRng ? mU2: mRng.uniform();
    //gaps_printf("u1=%f, u2=%f\n", mU1, mU2);
    mUseCachedRng = false;

    if (mMinAtoms < 2 && mMaxAtoms >= 2)
    {
        return false; // special indeterminate case
    }

    if (mMaxAtoms < 2)
    {
        return birth(domain); // always birth when no atoms exist
    }

    float lowerBound = deathProb(static_cast<double>(mMinAtoms));
    float upperBound = deathProb(static_cast<double>(mMaxAtoms));

    if (mU1 < 0.5f)
    {
        if (mU2 < lowerBound)
        {
            return death(domain);
        }
        if (mU2 >= upperBound)
        {
            return birth(domain);
        }
        return false; // can't determine B/D since range is too wide
    }
    //return false;
    //return (mU1 < 0.75f) ? move(domain) : exchange(domain);
    return exchange(domain);
}

bool ProposalQueue::birth(AtomicDomain &domain)
{
    AtomicProposal prop('B');
    uint64_t pos = domain.randomFreePosition(&(prop.rng), mUsedAtoms.vec());
    if (pos == 0)
    {
        DEBUG_PING // want to notify since this event should have near 0 probability
        GapsRng::rollBackOnce(); // ensure same proposal next time
        return false; // atom conflict, might have open spot if atom moves/dies
    }

    prop.r1 = (pos / mBinLength) / mNumCols;
    prop.c1 = (pos / mBinLength) % mNumCols;
    if (mUsedMatrixIndices.contains(prop.r1))
    {
        GapsRng::rollBackOnce(); // ensure same proposal next time
        return false; // matrix conflict - can't compute gibbs mass
    }
    prop.atom1 = domain.insert(pos, 0.f);

    mUsedMatrixIndices.insert(prop.r1);
    mUsedAtoms.insert(prop.atom1->pos);
    mQueue.push_back(prop);
    ++mMaxAtoms;
    return true;
}

bool ProposalQueue::death(AtomicDomain &domain)
{
    AtomicProposal prop('D');
    prop.atom1 = domain.randomAtom(&(prop.rng));
    prop.r1 = (prop.atom1->pos / mBinLength) / mNumCols;
    prop.c1 = (prop.atom1->pos / mBinLength) % mNumCols;

    if (mUsedMatrixIndices.contains(prop.r1))
    {
        GapsRng::rollBackOnce(); // ensure same proposal next time
        return false; // matrix conflict - can't compute gibbs mass or deltaLL
    }

    mUsedMatrixIndices.insert(prop.r1);
    mUsedAtoms.insert(prop.atom1->pos);
    mQueue.push_back(prop);
    --mMinAtoms;
    return true;
}

bool ProposalQueue::move(AtomicDomain &domain)
{
    AtomicProposal prop('M');
    AtomNeighborhood hood = domain.randomAtomWithNeighbors(&(prop.rng));

    uint64_t lbound = hood.hasLeft() ? hood.left->pos : 0;
    uint64_t rbound = hood.hasRight() ? hood.right->pos : mDomainLength;
    if (mUsedAtoms.contains(lbound) || mUsedAtoms.contains(rbound))
    {
        GapsRng::rollBackOnce(); // ensure same proposal next time
        return false; // atomic conflict - don't know neighbors
    }

    prop.pos = prop.rng.uniform64(lbound + 1, rbound - 1);
    prop.atom1 = hood.center;
    prop.r1 = (prop.atom1->pos / mBinLength) / mNumCols;
    prop.c1 = (prop.atom1->pos / mBinLength) % mNumCols;
    prop.r2 = (prop.pos / mBinLength) / mNumCols;
    prop.c2 = (prop.pos / mBinLength) % mNumCols;

    if (mUsedMatrixIndices.contains(prop.r1) || mUsedMatrixIndices.contains(prop.r2))
    {
        GapsRng::rollBackOnce(); // ensure same proposal next time
        return false; // matrix conflict - can't compute deltaLL
    }

    if (prop.r1 == prop.r2 && prop.c1 == prop.c2)
    {
        //prop.atom1->pos = prop.pos; // automatically accept moves in same bin
        return true;
    }

    mQueue.push_back(prop);
    mUsedMatrixIndices.insert(prop.r1);
    mUsedMatrixIndices.insert(prop.r2);
    mUsedAtoms.insert(prop.atom1->pos);
    mUsedAtoms.insert(prop.pos);
    return true;
}

bool ProposalQueue::exchange(AtomicDomain &domain)
{
    AtomicProposal prop('E');
    AtomNeighborhood hood = domain.randomAtomWithRightNeighbor(&(prop.rng));
    prop.atom1 = hood.center;
    prop.atom2 = hood.hasRight() ? hood.right : domain.front();

    prop.r1 = (prop.atom1->pos / mBinLength) / mNumCols;
    prop.c1 = (prop.atom1->pos / mBinLength) % mNumCols;
    prop.r2 = (prop.atom2->pos / mBinLength) / mNumCols;
    prop.c2 = (prop.atom2->pos / mBinLength) % mNumCols;

    if (mUsedMatrixIndices.contains(prop.r1) || mUsedMatrixIndices.contains(prop.r2))
    {
        GapsRng::rollBackOnce(); // ensure same proposal next time
        return false; // matrix conflict - can't compute deltaLL or gibbs mass
    }

    if (prop.r1 == prop.r2 && prop.c1 == prop.c2)
    {
        //float newMass = prop.rng.truncGammaUpper(a1->mass + a2->mass, 2.f, 1.f / mLambda);
        //float delta = (a1->mass > a2->mass) ? newMass - a1->mass : a2->mass - newMass;
        //if (a1->mass + delta > gaps::epsilon && a2->mass - delta > gaps::epsilon)
        //{
        //    a1->mass += delta;
        //    a2->mass -= delta;
        //}        
        return true; // TODO automatically accept exchanges in same bin
    }

    mQueue.push_back(prop);
    mUsedMatrixIndices.insert(prop.r1);
    mUsedMatrixIndices.insert(prop.r2);
    return true;
}

Archive& operator<<(Archive &ar, ProposalQueue &q)
{
    ar << q.mRng << q.mMinAtoms << q.mMaxAtoms << q.mBinLength << q.mNumCols
        << q.mAlpha << q.mDomainLength << q.mNumBins;
    return ar;
}

Archive& operator>>(Archive &ar, ProposalQueue &q)
{
    ar >> q.mRng >> q.mMinAtoms >> q.mMaxAtoms >> q.mBinLength >> q.mNumCols
        >> q.mAlpha >> q.mDomainLength >> q.mNumBins;
    return ar;
}