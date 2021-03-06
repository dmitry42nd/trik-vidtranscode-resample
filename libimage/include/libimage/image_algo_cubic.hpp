#ifndef TRIK_LIBIMAGE_IMAGE_ALGO_CUBIC_HPP_
#define TRIK_LIBIMAGE_IMAGE_ALGO_CUBIC_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <libimage/stdcpp.hpp>
#include <libimage/image_algo_resample_vh.hpp>


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace libimage /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


class AlgoInterpolationCubic : public BaseAlgoInterpolation1Dim<1, 2>
{
  public:
    AlgoInterpolationCubic(const float& _t)
    {
      assert(_t >= 0 && _t <= 1.0);

      const float t0 = 1;
      const float t1 = _t;
      const float t2 = _t*_t;
      const float t3 = _t*_t*_t;

      m_weight[0] = 0.5 * ( 0*t0 + -1*t1 +  2*t2 + -1*t3 );
      m_weight[1] = 0.5 * ( 2*t0 +  0*t1 + -5*t2 +  3*t3 );
      m_weight[2] = 0.5 * ( 0*t0 +  1*t1 +  4*t2 + -3*t3 );
      m_weight[3] = 0.5 * ( 0*t0 +  0*t1 + -1*t2 +  1*t3 );
    }

    template <typename PixelSetIn, typename PixelSetOut>
    bool operator()(const PixelSetIn& _pixelsIn,
                    PixelSetOut& _pixelsOut) const
    {
      typename PixelSetOut::Pixel result;

      for (size_t idx = 0; idx < s_weightDimension; ++idx)
        result += _pixelsIn[idx] * m_weight[idx];

      _pixelsOut.insertNewPixel() = result;

      return true;
    }

  private:
    static const size_t s_weightDimension = 4;

    /*
     * Cached data is:
     *                             [  0,  2,  0,  0 ]
     * 0,5 * [1, _t, _t^2, _t^3] * [ -1,  0,  1,  0 ]
     *                             [  2, -5,  4, -1 ]
     *                             [ -1,  3, -3,  1 ]
     */
    float m_weight[s_weightDimension];
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




template <typename _ImageIn, typename _ImageOut>
class ImageAlgorithm<BaseImageAlgorithm::AlgoResampleBicubic, _ImageIn, _ImageOut>
 : public BaseImageAlgorithm,
   public internal::AlgoResampleVH<internal::AlgoInterpolationCubic, internal::AlgoInterpolationCubic,
                                   _ImageIn, _ImageOut>
{
};


} /* **** **** **** **** **** * namespace libimage * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#endif // !TRIK_LIBIMAGE_IMAGE_ALGO_CUBIC_HPP_
