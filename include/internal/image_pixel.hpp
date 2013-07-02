#ifndef TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_PIXEL_HPP_
#define TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_PIXEL_HPP_

#ifndef __cplusplus
#error C++-only header
#endif


#include <climits>
#include <iostream>

#include "include/internal/stdcpp.hpp"


/* **** **** **** **** **** */ namespace trik /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace image /* **** **** **** **** **** */ {
/* **** **** **** **** **** */ namespace internal /* **** **** **** **** **** */ {


class BaseImagePixelAccessor
{
  protected:
    BaseImagePixelAccessor() {}

    template <typename UType>
    static UType utypeBitmask(size_t _size)
    {
      assert(_size <= sizeof(UType) * CHAR_BIT);
      return (static_cast<UType>(1u) << _size) - 1;
    }

    template <typename UType, bool _ofsPositive>
    static UType utypeGet(const UType& _value, size_t _size, size_t _ofs)
    {
      assert(_size <= sizeof(UType) * CHAR_BIT);
      assert(_ofs  <= sizeof(UType) * CHAR_BIT);

      if (_ofsPositive)
        return (_value >> _ofs) & utypeBitmask<UType>(_size);
      else
        return (_value & utypeBitmask<UType>(_size)) << _ofs;
    }

    template <typename UType, bool _ofsPositive>
    static UType utypeValue(const UType& _value, size_t _size, size_t _ofs)
    {
      assert(_size <= sizeof(UType) * CHAR_BIT);
      assert(_ofs  <= sizeof(UType) * CHAR_BIT);

      if (_ofsPositive)
        return (_value & utypeBitmask<UType>(_size)) << _ofs;
      else
        return (_value >> _ofs) & utypeBitmask<UType>(_size);
    }
};


} /* **** **** **** **** **** * namespace internal * **** **** **** **** **** */




class BaseImagePixel
{
  public:
    enum PixelType
    {
      PixelRGB565,
      PixelRGB888
    };

  protected:
    BaseImagePixel() {}
};


template <BaseImagePixel::PixelType PT>
class ImagePixel : public BaseImagePixel,
                   private internal::BaseImagePixelAccessor,
                   private assert_inst<false> // Generic instance, non-functional
{
};




class BaseImagePixelSet
{
  protected:
    BaseImagePixelSet() {}
};




template <typename BaseImagePixel::PixelType PT, size_t _pixelsCount>
class ImagePixelSet : public BaseImagePixelSet,
                      private assert_inst<(_pixelsCount > 0)> // sanity check
{
  public:
    typedef ImagePixel<PT> Pixel;

    ImagePixelSet()
     :BaseImagePixelSet(),
      m_pixels(),
      m_pixelFirst(0)
    {
    }

    void reset()
    {
      m_pixelFirst = 0;
      for (size_t pix = 0; pix < pixelsCount(); ++pix)
        m_pixels[pix] = Pixel();
    }

    size_t pixelsCount() const
    {
      return _pixelsCount;
    }

    Pixel& insertNewPixel()
    {
      const size_t currentPixel = m_pixelFirst;
      m_pixelFirst = pixelIndex(1);
      return m_pixels[currentPixel];
    }

    bool insertLastPixelCopy()
    {
      const Pixel cp = operator[](pixelsCount()-1);
      // must copy a pixel to be safe
      insertNewPixel() = cp;
      return true;
    }

    Pixel& operator[](size_t _index)
    {
      return m_pixels[pixelIndex(_index)];
    }

    const Pixel& operator[](size_t _index) const
    {
      return m_pixels[pixelIndex(_index)];
    }

  protected:
    size_t pixelIndex(size_t _index) const
    {
      return (m_pixelFirst + _index) % _pixelsCount;
    }

  private:
    Pixel  m_pixels[_pixelsCount];
    size_t m_pixelFirst;

    friend std::ostream& operator<<(std::ostream& _os, const ImagePixelSet& _s)
    {
      _os << "[" << _s.pixelsCount() << ":";
      for (size_t i = 0; i < _s.pixelsCount(); ++i)
        _os << " " << _s[i];
      _os << "]";
      return _os;
    }
};




template <typename PixelType1, typename PixelType2>
class ImagePixelConvertion
{
  public:
    ImagePixelConvertion() {}

    bool operator()(const PixelType1& _p1, PixelType2& _p2) const
    {
      float nr;
      float ng;
      float nb;

      return _p1.toNormalizedRGB(nr, ng, nb)
          && _p2.fromNormalizedRGB(nr, ng, nb);
    }
};

template <typename PixelType>
class ImagePixelConvertion<PixelType, PixelType> // specialization for same type copy
{
  public:
    ImagePixelConvertion() {}

    bool operator()(const PixelType& _p1, PixelType& _p2) const
    {
      _p2 = _p1;
      return true;
    }
};


template <typename PixelSetType1, typename PixelSetType2>
class ImagePixelSetConvertion
{
  public:
    ImagePixelSetConvertion() {}

    bool operator()(const PixelSetType1& _s1, PixelSetType2& _s2) const
    {
      ImagePixelConvertion<typename PixelSetType1::Pixel, typename PixelSetType2::Pixel> convertion;
      bool isOk = true;

      for (size_t i = 0; i < _s1.pixelsCount(); ++i)
        isOk &= convertion(_s1[i], _s2[i]);

      return isOk;
    }
};




} /* **** **** **** **** **** * namespace image * **** **** **** **** **** */
} /* **** **** **** **** **** * namespace trik * **** **** **** **** **** */


#include "include/internal/image_pixel_rgb.hpp"


#endif // !TRIK_VIDEO_RESAMPLE_INTERNAL_IMAGE_PIXEL_HPP_
