#include <algorithm>
#include <limits>
#include <cmath>

namespace CopyResize {

template<class FloatingType = float, class ElementT, typename InputFunctor, typename OutputFunctor>
inline void Bilinear(size_t input_width, size_t input_height, InputFunctor && image, size_t output_width,  size_t output_height, OutputFunctor && output)
{
    
    auto ratiox = FloatingType(input_width) / FloatingType(output_width);
    auto ratioy = FloatingType(input_height) / FloatingType(output_height);

    for (int i = 0; i < output_height; i++) {
        const FloatingType iF = i;
        const int y_l = std::floor(ratioy * iF);
        const int y_h = std::min<int>( std::ceil(ratioy * iF), input_height-1);
        const auto y_weight = (ratioy * iF) - y_l;
        const auto y_weight_inv = FloatingType(1.0) - y_weight;
        for (int j = 0; j < output_width; j++) {
            const FloatingType jF = j;
            const int x_l = std::floor(ratiox * jF);
            const int x_h = std::min<int>( std::ceil(ratiox * jF), input_width-1);

            const auto x_weight = (ratiox * jF) - x_l;
            const auto x_weight_inv = FloatingType(1.0) - x_weight;

            const auto a = image(x_l,y_l); //data[(int)y_l * input_width + (int)x_l];
            const auto b = image(x_h,y_l); //data[(int)y_l * input_width + (int)x_h];
            const auto c = image(x_l,y_h); //data[(int)y_h * input_width + (int)x_l];
            const auto d = image(x_h,y_h); //data[(int)y_h * input_width + (int)x_h];

            output(j,i, a * x_weight_inv * y_weight_inv +
                        b * x_weight * y_weight_inv +
                        c * y_weight * x_weight_inv +
                        d * x_weight * y_weight);
        }
    }
}

    template<class InputT1, class InputT2>
    constexpr static auto cubicPolate(const InputT1 v0, const InputT1 v1, const InputT1 v2, const InputT1 v3, const InputT2 frac)
    {
        auto A = (v3-v2)-(v0-v1);
        auto B = (v0-v1)-A;
        auto C = v2-v0;
        auto D = v1;
        return D + frac * (C + frac * (B + frac * A));
        //return D + (C + (B + A * frac) *  frac ) * frac;
    }

    template<class InputT = float, class ElementT>
    constexpr static auto bicubicPolate(const ElementT* const ndata, const InputT fracx, const InputT fracy)
    {
        auto x1 = cubicPolate( ndata[0], ndata[1], ndata[2], ndata[3], fracx );
        auto x2 = cubicPolate( ndata[4], ndata[5], ndata[6], ndata[7], fracx );
        auto x3 = cubicPolate( ndata[8], ndata[9], ndata[10], ndata[11], fracx );
        auto x4 = cubicPolate( ndata[12], ndata[13], ndata[14], ndata[15], fracx );

        return cubicPolate( x1, x2, x3, x4, fracy );
    }

    //  copyResizeBicubic template function implementation
    template<class FloatingType = float, class ElementT, typename InputFunctor, typename OutputFunctor>
    inline void Bicubic(size_t src_width, size_t src_height, InputFunctor && image, size_t width,  size_t height, OutputFunctor && output)
    {

        auto ratiox = FloatingType(src_width) / FloatingType(width);
        auto ratioy = FloatingType(src_height) / FloatingType(height);

        for (size_t y = 0; y < height; ++y)
        {
            for (size_t x = 0; x < width; ++x)
            {
                FloatingType xMappingToOrigin = FloatingType(x) * ratiox;
                FloatingType yMappingToOrigin = FloatingType(y) * ratioy;
                FloatingType xMappingToOriginFloor = std::floor(xMappingToOrigin);
                FloatingType yMappingToOriginFloor = std::floor(yMappingToOrigin);
                FloatingType xMappingToOriginFrac = xMappingToOrigin - xMappingToOriginFloor;
                FloatingType yMappingToOriginFrac = yMappingToOrigin - yMappingToOriginFloor;

                ElementT ndata[4 * 4];
                for (int ndatay = -1; ndatay <= 2; ++ndatay)
                {
                    for (int ndatax = -1; ndatax <= 2; ++ndatax)
                    {
                        const size_t src_x = std::clamp<std::ptrdiff_t>(xMappingToOriginFloor + ndatax, 0, src_width-1);
                        const size_t src_y = std::clamp<std::ptrdiff_t>(yMappingToOriginFloor + ndatay, 0, src_height-1);
                        ndata[(ndatay + 1) * 4 + (ndatax + 1)] = image(src_x, src_y);
                    }

                }
                output(x,y, bicubicPolate(ndata, xMappingToOriginFrac, yMappingToOriginFrac) );
            }
        }
    }

}
