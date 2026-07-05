#pragma once

#include <array>
#include <cmath>
#include <algorithm>

class c_color
{
private:
    union
    {
        std::uint8_t rgba[4];
        std::uint32_t as_u32;
    } colors;

public:
    c_color()
    {
        u32() = 0xFFFFFFFF;
    }

    c_color(std::uint32_t color32)
    {
        u32() = color32;
    }

    c_color(int _r, int _g, int _b)
    {
        set((std::uint8_t)_r, (std::uint8_t)_g, (std::uint8_t)_b, 255);
    }

    c_color(int _r, int _g, int _b, int _a)
    {
        set((std::uint8_t)_r, (std::uint8_t)_g, (std::uint8_t)_b, (std::uint8_t)_a);
    }

    void set(std::uint32_t value)
    {
        u32() = value;
    }

    void set(std::uint8_t _r, std::uint8_t _g, std::uint8_t _b, std::uint8_t _a = 0xFF)
    {
        colors.rgba[0] = std::clamp< std::uint8_t >(_r, 0u, 255u);
        colors.rgba[1] = std::clamp< std::uint8_t >(_g, 0u, 255u);
        colors.rgba[2] = std::clamp< std::uint8_t >(_b, 0u, 255u);
        colors.rgba[3] = std::clamp< std::uint8_t >(_a, 0u, 255u);
    }

    void get(int& _r, int& _g, int& _b, int& _a) const
    {
        _r = colors.rgba[0];
        _g = colors.rgba[1];
        _b = colors.rgba[2];
        _a = colors.rgba[3];
    }

    void get(int& _r, int& _g, int& _b) const
    {
        _r = colors.rgba[0];
        _g = colors.rgba[1];
        _b = colors.rgba[2];
    }

    std::uint8_t& r()
    {
        return colors.rgba[0];
    }

    std::uint8_t& g()
    {
        return colors.rgba[1];
    }

    std::uint8_t& b()
    {
        return colors.rgba[2];
    }

    std::uint8_t& a()
    {
        return colors.rgba[3];
    }

    std::uint8_t r() const
    {
        return colors.rgba[0];
    }

    std::uint8_t g() const
    {
        return colors.rgba[1];
    }

    std::uint8_t b() const
    {
        return colors.rgba[2];
    }

    std::uint8_t a() const
    {
        return colors.rgba[3];
    }

    std::uint8_t& operator[](int index)
    {
        return colors.rgba[index];
    }

    const std::uint8_t& operator[](int index) const
    {
        return colors.rgba[index];
    }

    bool operator==(const c_color& other) const
    {
        return other.r() == r() && other.g() == g() && other.b() == b() && other.a() == a();
    }

    bool operator!=(const c_color& other) const
    {
        return !(*this == other);
    }

    c_color& operator=(const c_color& other)
    {
        u32() = other.u32();
        return *this;
    }

    std::uint32_t& u32()
    {
        return colors.as_u32;
    }

    std::uint32_t u32() const
    {
        return colors.as_u32;
    }

    double hue() const
    {
        double r = colors.rgba[0] / 255.f;
        double g = colors.rgba[1] / 255.f;
        double b = colors.rgba[2] / 255.f;

        double mx = std::max< double >(r, std::max< double >(g, b));
        double mn = std::min< double >(r, std::min< double >(g, b));
        if (mx == mn)
            return 0.f;

        double delta = mx - mn;

        double hue = 0.f;
        if (mx == r)
            hue = (g - b) / delta;
        else if (mx == g)
            hue = 2.f + (b - r) / delta;
        else
            hue = 4.f + (r - g) / delta;

        hue *= 60.f;
        if (hue < 0.f)
            hue += 360.f;

        return hue / 360.f;
    }

    double saturation() const
    {
        double r = colors.rgba[0] / 255.f;
        double g = colors.rgba[1] / 255.f;
        double b = colors.rgba[2] / 255.f;

        double mx = std::max< double >(r, std::max< double >(g, b));
        double mn = std::min< double >(r, std::min< double >(g, b));

        double delta = mx - mn;

        if (mx == 0.f)
            return delta;

        return delta / mx;
    }

    double brightness() const
    {
        double r = colors.rgba[0] / 255.f;
        double g = colors.rgba[1] / 255.f;
        double b = colors.rgba[2] / 255.f;

        return std::max< double >(r, std::max< double >(g, b));
    }

    static c_color hsb(float hue, float saturation, float brightness)
    {
        hue = std::clamp< float >(hue, 0.f, 1.f);
        saturation = std::clamp< float >(saturation, 0.f, 1.f);
        brightness = std::clamp< float >(brightness, 0.f, 1.f);

        float h = (hue == 1.f) ? 0.f : (hue * 6.f);
        float f = h - static_cast<int>(h);
        float p = brightness * (1.f - saturation);
        float q = brightness * (1.f - saturation * f);
        float t = brightness * (1.f - (saturation * (1.f - f)));

        if (h < 1.f)
            return c_color((int)(brightness * 255), (int)(t * 255), (int)(p * 255));
        else if (h < 2.f)
            return c_color((int)(q * 255), (int)(brightness * 255), (int)(p * 255));
        else if (h < 3.f)
            return c_color((int)(p * 255), (int)(brightness * 255), (int)(t * 255));
        else if (h < 4)
            return c_color((int)(p * 255), (int)(q * 255), (int)(brightness * 255));
        else if (h < 5)
            return c_color((int)(t * 255), (int)(p * 255), (int)(brightness * 255));
        else
            return c_color((int)(brightness * 255), (int)(p * 255), (int)(q * 255));
    }

    c_color multiply(const c_color& other, float strength) const
    {
        if (*this == other)
            return *this;

        return
        {
            static_cast<int>(std::lerp(static_cast<float>(colors.rgba[0]), static_cast<float>(other.r()), strength)),
            static_cast<int>(std::lerp(static_cast<float>(colors.rgba[1]), static_cast<float>(other.g()), strength)),
            static_cast<int>(std::lerp(static_cast<float>(colors.rgba[2]), static_cast<float>(other.b()), strength)),
            static_cast<int>(std::lerp(static_cast<float>(colors.rgba[3]), static_cast<float>(other.a()), strength)),
        };
    }

    c_color new_alpha(int alpha) const {
        return c_color(colors.rgba[0], colors.rgba[1], colors.rgba[2], std::clamp(alpha, 0, 255));
    }

    c_color modify_alpha(float alpha) {
        return c_color(colors.rgba[0], colors.rgba[1], colors.rgba[2], std::clamp(static_cast<int>(colors.rgba[3] * alpha), 0, 255));
    }

    c_color increase(int value, bool consider_alpha = false) const
    {
        return c_color(colors.rgba[0] + value, colors.rgba[1] + value, colors.rgba[2] + value, colors.rgba[3] + consider_alpha * value);
    }

    c_color decrease(int value, bool consider_alpha = false) const
    {
        return increase(-value, consider_alpha);
    }

    c_color grayscale()
    {
        auto x = static_cast<int>(0.299f * colors.rgba[0] + 0.587f * colors.rgba[1] + 0.114f * colors.rgba[2]);
        return c_color(x, x, x, colors.rgba[3]);
    }
};