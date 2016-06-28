#ifndef GRAPH_SLAM_UK_SLAM2D_POLICY
#define GRAPH_SLAM_UK_SLAM2D_POLICY

#include <Eigen/Dense>
#include <math.h>
#include <graph_slam_uk/utils/eigen_tools.h>
struct Slam2d_Policy
{
private:
  // typedef Eigen::Matrix<double, 3, 3> TransformMatrixRaw;
public:
  // change sizes to multiples of 2 for use of vector instructions
  typedef size_t Id;
  typedef Eigen::Matrix<double, 3, 3> JacobianMatrix;
  typedef Eigen::Matrix<double, 3, 3> InformMatrix;
  typedef Eigen::Matrix<double, 3, 3> CovarMatrix;
  typedef Eigen::Matrix<double, 3, 1> ErrorVector;
  typedef std::pair<JacobianMatrix, JacobianMatrix> JacobianPair;
  typedef Eigen::Matrix<double, 3, 1> Pose;
  typedef Eigen::Transform<double, 2, Eigen::TransformTraits::Affine>
      TransformMatrix;

  // enum Modes{M2D,M3D};
  // static const Modes dimmention = M2D;
  static const size_t block_size = 3;  // change to 6 for 3D world

  static TransformMatrix vecToTransMat(const Pose &p)
  {
    return eigt::getTransFromPose(p);

    //  TransformMatrixRaw mat;
    // // Pose p = std::forward<const Pose>(pose);
    // //Pose  p= pose;
    //  mat<<cos(p(2)), -sin(p(2)), p(0),
    //       sin(p(2)), cos(p(2)),  p(1),
    //       0,             0,              1;
    //  return TransformMatrix(mat);
  }

  static Pose transMatToVec(const TransformMatrix &trans)
  {
    return eigt::getPoseFromTransform(trans);
    // Pose pose;
    // pose<<trans(0,2),trans(1,2), atan2(trans(1,0),trans(0,0));
    // return pose;
  }

  static JacobianPair calcJacobianBlocks(const Pose &xi, const Pose &xj,
                                         const Pose &trans)
  {
    JacobianMatrix aij = JacobianMatrix::Ones();
    JacobianMatrix bij = JacobianMatrix::Ones();
    double angle = trans(2) + xi(2);
    auto dist = xj - xi;
    double co = cos(angle);
    double si = sin(angle);
    aij(0, 0) = -co;
    aij(0, 1) = -si;
    aij(0, 2) = -si * dist(0) + co * dist(1);
    aij(1, 0) = si;
    aij(1, 1) = -co;
    aij(1, 2) = -co * dist(0) - si * dist(1);
    aij(2, 0) = 0;
    aij(2, 1) = 0;
    aij(2, 2) = -1;

    bij(0, 0) = co;
    bij(0, 1) = si;
    bij(0, 2) = 0;
    bij(1, 0) = -si;
    bij(1, 1) = co;
    bij(1, 2) = 0;
    bij(2, 0) = 0;
    bij(2, 1) = 0;
    bij(2, 2) = 1;

    return std::make_pair(std::move(aij), std::move(bij));
  }

  static ErrorVector calcError(const Pose &x, const Pose &y, const Pose &trans)
  {
    TransformMatrix xi = vecToTransMat(x);
    TransformMatrix xj = vecToTransMat(y);
    TransformMatrix zij = vecToTransMat(trans);
    return transMatToVec(zij.inverse() * (xi.inverse() * xj));
  }

  static Pose addPoses(const Pose &a, const Pose &b)
  {
    Pose ret;
    ret = a + b;
    ret(2) = eigt::normalizeAngle(a(2) + b(2));
    return ret;
  }
};

#endif
