#pragma once

namespace Cast
{
    namespace Detail
    {
        template<typename T, T V>
        struct IntegralConstant
        {
            static constexpr T Value = V;
        };

        using TrueType = IntegralConstant<bool, true>;
        using FalseType = IntegralConstant<bool, false>;

        template<bool B, typename T = void>
        struct EnableIf
        {
        };

        template<typename T>
        struct EnableIf<true, T>
        {
            using Type = T;
        };

        template<bool B, typename T, typename F>
        struct Conditional
        {
            using Type = T;
        };

        template<typename T, typename F>
        struct Conditional<false, T, F>
        {
            using Type = F;
        };

        template<typename T>
        struct RemoveReference
        {
            using Type = T;
        };

        template<typename T>
        struct RemoveReference<T&>
        {
            using Type = T;
        };

        template<typename T>
        struct RemoveReference<T&&>
        {
            using Type = T;
        };

        template<typename T>
        struct RemoveConst
        {
            using Type = T;
        };

        template<typename T>
        struct RemoveConst<const T>
        {
            using Type = T;
        };

        template<typename T>
        struct RemoveVolatile
        {
            using Type = T;
        };

        template<typename T>
        struct RemoveVolatile<volatile T>
        {
            using Type = T;
        };

        template<typename T>
        struct RemoveCv
        {
            using Type = typename RemoveConst<typename RemoveVolatile<T>::Type>::Type;
        };

        template<typename T>
        struct RemoveCvRef
        {
            using Type = typename RemoveCv<typename RemoveReference<T>::Type>::Type;
        };

        template<typename T, typename U>
        struct IsSame : FalseType
        {
        };

        template<typename T>
        struct IsSame<T, T> : TrueType
        {
        };

        template<typename T>
        struct IsReference : FalseType
        {
        };

        template<typename T>
        struct IsReference<T&> : TrueType
        {
        };

        template<typename T>
        struct IsReference<T&&> : TrueType
        {
        };

        template<typename T>
        struct IsLValueReference : FalseType
        {
        };

        template<typename T>
        struct IsLValueReference<T&> : TrueType
        {
        };

        template<typename T>
        struct IsPointer : FalseType
        {
        };

        template<typename T>
        struct IsPointer<T*> : TrueType
        {
        };

        template<typename T>
        struct IsPointer<T* const> : TrueType
        {
        };

        template<typename T>
        struct IsPointer<T* volatile> : TrueType
        {
        };

        template<typename T>
        struct IsPointer<T* const volatile> : TrueType
        {
        };

        template<typename T>
        struct IsIntegral : FalseType
        {
        };

        template<> struct IsIntegral<bool> : TrueType {};
        template<> struct IsIntegral<char> : TrueType {};
        template<> struct IsIntegral<signed char> : TrueType {};
        template<> struct IsIntegral<unsigned char> : TrueType {};
        template<> struct IsIntegral<short> : TrueType {};
        template<> struct IsIntegral<unsigned short> : TrueType {};
        template<> struct IsIntegral<int> : TrueType {};
        template<> struct IsIntegral<unsigned int> : TrueType {};
        template<> struct IsIntegral<long> : TrueType {};
        template<> struct IsIntegral<unsigned long> : TrueType {};
        template<> struct IsIntegral<long long> : TrueType {};
        template<> struct IsIntegral<unsigned long long> : TrueType {};

        template<typename T>
        struct IsFloat : FalseType
        {
        };

        template<> struct IsFloat<float> : TrueType {};
        template<> struct IsFloat<double> : TrueType {};
        template<> struct IsFloat<long double> : TrueType {};

        template<typename T>
        struct IsArithmetic : IntegralConstant<bool, IsIntegral<typename RemoveCv<T>::Type>::Value || IsFloat<typename RemoveCv<T>::Type>::Value>
        {
        };

        template<typename T>
        auto DeclVal() -> T&&;

        template<typename TTo, typename TFrom>
        static auto TestStaticCast(int) -> decltype(static_cast<TTo>(DeclVal<TFrom>()), TrueType{})
        {
            return {};
        }

        template<typename, typename>
        static auto TestStaticCast(...) -> FalseType
        {
            return {};
        }

        template<typename TTo, typename TFrom>
        struct IsStaticCastable : decltype(TestStaticCast<TTo, TFrom>(0))
        {
        };

        template<typename TTo, typename TFrom>
        struct CanBitCopy : IntegralConstant<bool, sizeof(TTo) == sizeof(TFrom) && !IsReference<TTo>::Value && !IsReference<TFrom>::Value>
        {
        };

        template<typename TTo, typename TFrom>
        auto BitCopy(const TFrom& Value) -> TTo
        {
            TTo Result{};

            auto* Dst = reinterpret_cast<unsigned char*>(&Result);
            auto* Src = reinterpret_cast<const unsigned char*>(&Value);

            for (unsigned long long i = 0; i < sizeof(TTo); ++i)
            {
                Dst[i] = Src[i];
            }

            return Result;
        }
    }

    template<typename TTo, typename TFrom>
    constexpr auto To(TFrom&& Value) -> decltype(auto)
    {
        using RawTo = typename Detail::RemoveCvRef<TTo>::Type;
        using RawFrom = typename Detail::RemoveCvRef<TFrom>::Type;

        if constexpr (Detail::IsReference<TTo>::Value)
        {
            static_assert(Detail::IsLValueReference<TFrom&&>::Value, "Cannot cast rvalue to reference");
            return reinterpret_cast<TTo>(Value);
        }
        else if constexpr (Detail::IsSame<RawTo, RawFrom>::Value)
        {
            return static_cast<TTo>(Value);
        }
        else if constexpr (Detail::IsPointer<RawTo>::Value && Detail::IsPointer<RawFrom>::Value)
        {
            return reinterpret_cast<TTo>(Value);
        }
        else if constexpr (Detail::IsIntegral<RawTo>::Value && Detail::IsPointer<RawFrom>::Value)
        {
            return reinterpret_cast<TTo>(Value);
        }
        else if constexpr (Detail::IsPointer<RawTo>::Value && Detail::IsIntegral<RawFrom>::Value)
        {
            return reinterpret_cast<TTo>(Value);
        }
        else if constexpr (Detail::IsArithmetic<RawTo>::Value && Detail::IsArithmetic<RawFrom>::Value)
        {
            return static_cast<TTo>(Value);
        }
        else if constexpr (Detail::IsStaticCastable<TTo, TFrom&&>::Value)
        {
            return static_cast<TTo>(Value);
        }
        else if constexpr (Detail::CanBitCopy<RawTo, RawFrom>::Value)
        {
            return Detail::BitCopy<RawTo>(Value);
        }
        else
        {
            static_assert(sizeof(TFrom) == 0, "Cast::To<TTo>(Value): unsupported cast");
        }
    }
}