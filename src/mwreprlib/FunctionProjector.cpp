#include "FunctionProjector.h"
#include "RepresentableFunction.h"
#include "FunctionTree.h"
#include "MWAdaptor.h"
#include "MWNode.h"
#include "ScalingBasis.h"
#include "QuadratureCache.h"

using namespace std;
using namespace Eigen;

template<int D>
FunctionProjector<D>::FunctionProjector(const MWAdaptor<D> &a, int iter) 
        : MWProjector<D>(a, iter) {
    this->inpFunc = 0;
}

template<int D>
FunctionProjector<D>::~FunctionProjector() {
    if (this->inpFunc != 0) {
        MSG_ERROR("Projector not properly cleared");
    }
}

template<int D>
void FunctionProjector<D>::operator()(FunctionTree<D> &out,
                                      RepresentableFunction<D> &inp) {
    this->inpFunc = &inp;
    this->buildTree(out);
    out.mwTransformUp();
    this->inpFunc = 0;
}

template<int D>
void FunctionProjector<D>::calcNode(MWNode<D> &node) {
    const ScalingBasis &sf = node.getMWTree().getScalingFunctions();
    if (sf.getType() != Interpol) {
        NOT_IMPLEMENTED_ABORT;
    }
    int quadratureOrder = sf.getQuadratureOrder();
    getQuadratureCache(qc);
    const VectorXd &pts = qc.getRoots(quadratureOrder);
    const VectorXd &wgts = qc.getWeights(quadratureOrder);

    VectorXd &tmpvec = node.getMWTree().getTmpScalingVector();

    int scale = node.getScale();
    int tDim = 1 << D;
    int kp1_d = node.getKp1_d();

    double scaleFactor = 1.0 / pow(2.0, scale + 1.0);
    double sqrtScaleFactor = sqrt(scaleFactor);
    double point[D];

    for (int cIdx = 0; cIdx < tDim; cIdx++) {
        NodeIndex<D> nIdx(node.getNodeIndex(), cIdx);
        const int *l = nIdx.getTranslation();

        int indexCounter[D];
        for (int i = 0; i < D; i++) {
            indexCounter[i] = 0;
        }

        for (int i = 0; i < kp1_d; i++) {
            double coef = 1.0;
            for (int j = 0; j < D; j++) {
                point[j] = scaleFactor * (pts(indexCounter[j]) + l[j]);
                coef *= sqrt(wgts(indexCounter[j])) * sqrtScaleFactor;
            }

            tmpvec(i) = coef * this->inpFunc->evalf(point);

            indexCounter[0]++;
            for (int j = 0; j < D - 1; j++) {
                if (indexCounter[j] == quadratureOrder) {
                    indexCounter[j] = 0;
                    indexCounter[j + 1]++;
                }
            }
        }
        node.getCoefs().segment(cIdx * kp1_d, kp1_d) = tmpvec;
    }
    node.mwTransform(Compression);
    node.setHasCoefs();
    node.calcNorms();
}

template class FunctionProjector<1>;
template class FunctionProjector<2>;
template class FunctionProjector<3>;
