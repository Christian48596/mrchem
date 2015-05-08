#include "MWAdaptor.h"
#include "MWTree.h"
#include "MWNode.h"
#include "constants.h"

using namespace std;

template<int D>
MWAdaptor<D>::MWAdaptor(double pr, bool abs, int scale) {
    this->maxScale = scale;
    this->absPrec = abs;
    this->prec = pr;
}

template<int D>
MRNodeVector* MWAdaptor<D>::splitNodeVector(MRNodeVector &nodeVec, 
        MRNodeVector *noSplit) const {
    MRNodeVector *split = new MRNodeVector;
    for (int n = 0; n < nodeVec.size(); n++) {
        MWNode<D> &node = static_cast<MWNode<D> &>(*nodeVec[n]);
        if (splitNode(node)) {
            split->push_back(&node);
        } else if (noSplit != 0) {
            noSplit->push_back(&node);
        }
    }
    return split;
}

template<int D>
bool MWAdaptor<D>::splitNode(MWNode<D> &node) const {
    if (this->prec < 0.0) {
        return false;
    }
    int scale = node.getScale();
    if ((scale + 1) >= this->maxScale) {
        println(10, "Maximum scale reached: " << scale + 1);
        return false;
    }
    double t_norm = 1.0;
    if (not this->absPrec) {
        t_norm = sqrt(node.getMWTree().getSquareNorm());
    }
    double w_norm = node.getWaveletNorm();
    double thrs = getWaveletThreshold(t_norm, scale);

    if (w_norm > thrs) {
        return true;
    }
    return false;
}

/** Calculate the threshold for the wavelet norm.
  *
  * Calculates the threshold that has to be met in the wavelet norm in order to
  * guarantee the precision in the function representation. Depends on the
  * square norm of the function and the requested relative accuracy. */
template<int D>
double MWAdaptor<D>::getWaveletThreshold(double norm, int scale) const {
    double expo = (0.5 * (scale + 1));
    double thrs_1 = 2.0 * MachinePrec;
    double thrs_2 = norm * this->prec * pow(2.0, -expo);
    return max(thrs_1, thrs_2);
}

template class MWAdaptor<1>;
template class MWAdaptor<2>;
template class MWAdaptor<3>;
