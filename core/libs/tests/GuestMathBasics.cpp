#include "prx/libc/include/general/VabiMacros.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>

extern "C" {
int APS5_VABI __isfinite_nid_postfix(double);
int APS5_VABI __isnan_nid_postfix(double);
int APS5_VABI __signbit_nid_postfix(double);
int APS5_VABI __isnanf_nid_postfix(float);
int APS5_VABI __signbitf_nid_postfix(float);
double APS5_VABI fmod_nid_postfix(double, double);
float APS5_VABI remainderf_nid_postfix(float, float);
float APS5_VABI roundf_nid_postfix(float);
double APS5_VABI modf_nid_postfix(double, double*);
float APS5_VABI modff_nid_postfix(float, float*);
float APS5_VABI ldexpf_nid_postfix(float, int);
float APS5_VABI exp2f_nid_postfix(float);
float APS5_VABI log2f_nid_postfix(float);
float APS5_VABI expf_nid_postfix(float);
float APS5_VABI logf_nid_postfix(float);
float APS5_VABI powf_nid_postfix(float, float);
float APS5_VABI sinf_nid_postfix(float);
float APS5_VABI cosf_nid_postfix(float);
float APS5_VABI atanf_nid_postfix(float);
float APS5_VABI tanhf_nid_postfix(float);
double APS5_VABI cbrt_nid_postfix(double);
float APS5_VABI cbrtf_nid_postfix(float);
double APS5_VABI log2_nid_postfix(double);
double APS5_VABI log10_nid_postfix(double);
double APS5_VABI tan_nid_postfix(double);
double APS5_VABI tanh_nid_postfix(double);
double APS5_VABI atan_nid_postfix(double);
double APS5_VABI atan2_nid_postfix(double, double);
double APS5_VABI asin_nid_postfix(double);
double APS5_VABI acos_nid_postfix(double);
void APS5_VABI sincos_nid_postfix(double, double*, double*);
void APS5_VABI sincosf_nid_postfix(float, float*, float*);
float APS5_VABI _FSinh_nid_postfix(float, float);
float APS5_VABI _FCosh_nid_postfix(float, float);
}

namespace {

constexpr double Pi = 3.14159265358979323846;
constexpr double Infinity = std::numeric_limits<double>::infinity();
constexpr float InfinityF = std::numeric_limits<float>::infinity();
constexpr double NotANumber = std::numeric_limits<double>::quiet_NaN();

void Require(bool condition, int line) {
    if (!condition) {
        std::fprintf(stderr, "Guest math basics check failed at line %d\n", line);
        std::abort();
    }
}

#define REQUIRE(condition) Require((condition), __LINE__)

bool Near(double actual, double expected, double relative) {
    return std::fabs(actual - expected) <= relative * std::fmax(std::fabs(expected), 1.0);
}

void CheckClassification() {
    REQUIRE(__isfinite_nid_postfix(1.5) == 1 && __isfinite_nid_postfix(0.0) == 1);
    REQUIRE(__isfinite_nid_postfix(std::numeric_limits<double>::denorm_min()) == 1);
    REQUIRE(__isfinite_nid_postfix(Infinity) == 0 && __isfinite_nid_postfix(-Infinity) == 0);
    REQUIRE(__isfinite_nid_postfix(NotANumber) == 0);
    REQUIRE(__isnan_nid_postfix(NotANumber) == 1 && __isnan_nid_postfix(Infinity) == 0 && __isnan_nid_postfix(0.0) == 0);
    REQUIRE(__signbit_nid_postfix(-0.0) == 1 && __signbit_nid_postfix(0.0) == 0);
    REQUIRE(__signbit_nid_postfix(-Infinity) == 1 && __signbit_nid_postfix(2.0) == 0);
    REQUIRE(__isnanf_nid_postfix(std::numeric_limits<float>::quiet_NaN()) == 1);
    REQUIRE(__isnanf_nid_postfix(InfinityF) == 0 && __isnanf_nid_postfix(1.0f) == 0);
    REQUIRE(__signbitf_nid_postfix(-0.0f) == 1 && __signbitf_nid_postfix(0.0f) == 0 && __signbitf_nid_postfix(-1.0f) == 1);
}

void CheckExactArithmetic() {
    REQUIRE(fmod_nid_postfix(5.5, 2.0) == 1.5);
    REQUIRE(fmod_nid_postfix(-5.5, 2.0) == -1.5);
    REQUIRE(fmod_nid_postfix(0.0, 2.0) == 0.0);
    REQUIRE(std::isnan(fmod_nid_postfix(5.0, 0.0)));
    REQUIRE(std::isnan(fmod_nid_postfix(Infinity, 2.0)));
    REQUIRE(fmod_nid_postfix(5.0, Infinity) == 5.0);

    REQUIRE(remainderf_nid_postfix(5.0f, 3.0f) == -1.0f);
    REQUIRE(remainderf_nid_postfix(7.0f, 2.0f) == -1.0f);
    REQUIRE(remainderf_nid_postfix(5.0f, 2.0f) == 1.0f);
    REQUIRE(std::isnan(remainderf_nid_postfix(1.0f, 0.0f)));

    REQUIRE(roundf_nid_postfix(2.5f) == 3.0f && roundf_nid_postfix(-2.5f) == -3.0f);
    REQUIRE(roundf_nid_postfix(2.4999f) == 2.0f && roundf_nid_postfix(0.4f) == 0.0f);
    REQUIRE(roundf_nid_postfix(-0.4f) == 0.0f && __signbitf_nid_postfix(roundf_nid_postfix(-0.4f)) == 1);

    double whole = 0;
    REQUIRE(modf_nid_postfix(3.75, &whole) == 0.75 && whole == 3.0);
    REQUIRE(modf_nid_postfix(-3.75, &whole) == -0.75 && whole == -3.0);
    REQUIRE(modf_nid_postfix(5.0, &whole) == 0.0 && whole == 5.0);
    REQUIRE(modf_nid_postfix(Infinity, &whole) == 0.0 && whole == Infinity);
    float wholeF = 0;
    REQUIRE(modff_nid_postfix(3.75f, &wholeF) == 0.75f && wholeF == 3.0f);
    REQUIRE(modff_nid_postfix(-0.5f, &wholeF) == -0.5f && __signbitf_nid_postfix(wholeF) == 1 && wholeF == 0.0f);

    REQUIRE(ldexpf_nid_postfix(1.5f, 4) == 24.0f);
    REQUIRE(ldexpf_nid_postfix(1.0f, -1) == 0.5f);
    REQUIRE(ldexpf_nid_postfix(3.0f, 0) == 3.0f);
    REQUIRE(ldexpf_nid_postfix(1.0f, 200) == InfinityF);
    REQUIRE(ldexpf_nid_postfix(0.0f, 10) == 0.0f);

    REQUIRE(exp2f_nid_postfix(3.0f) == 8.0f && exp2f_nid_postfix(-1.0f) == 0.5f && exp2f_nid_postfix(0.0f) == 1.0f);
    REQUIRE(log2f_nid_postfix(8.0f) == 3.0f && log2f_nid_postfix(1.0f) == 0.0f);
    REQUIRE(log2_nid_postfix(1024.0) == 10.0);
    REQUIRE(expf_nid_postfix(0.0f) == 1.0f && logf_nid_postfix(1.0f) == 0.0f);
    REQUIRE(powf_nid_postfix(2.0f, 10.0f) == 1024.0f && powf_nid_postfix(5.0f, 0.0f) == 1.0f);
    REQUIRE(powf_nid_postfix(std::numeric_limits<float>::quiet_NaN(), 0.0f) == 1.0f);
    REQUIRE(sinf_nid_postfix(0.0f) == 0.0f && cosf_nid_postfix(0.0f) == 1.0f);
    REQUIRE(__signbitf_nid_postfix(sinf_nid_postfix(-0.0f)) == 1);
    REQUIRE(tan_nid_postfix(0.0) == 0.0 && tanh_nid_postfix(0.0) == 0.0);
    REQUIRE(tanh_nid_postfix(Infinity) == 1.0 && tanh_nid_postfix(-Infinity) == -1.0);
    REQUIRE(tanhf_nid_postfix(InfinityF) == 1.0f && tanhf_nid_postfix(-InfinityF) == -1.0f);
    REQUIRE(acos_nid_postfix(1.0) == 0.0 && asin_nid_postfix(0.0) == 0.0 && atan_nid_postfix(0.0) == 0.0);
    REQUIRE(atan2_nid_postfix(0.0, -1.0) == Pi);
    REQUIRE(atan2_nid_postfix(-0.0, -1.0) == -Pi);
    REQUIRE(atan2_nid_postfix(0.0, 1.0) == 0.0 && atan2_nid_postfix(0.0, 0.0) == 0.0);
    REQUIRE(std::isnan(acos_nid_postfix(2.0)) && std::isnan(asin_nid_postfix(-2.0)));
    REQUIRE(log10_nid_postfix(1.0) == 0.0);
    REQUIRE(_FSinh_nid_postfix(0.0f, 5.0f) == 0.0f);
    REQUIRE(_FCosh_nid_postfix(0.0f, 5.0f) == 5.0f);
}

void CheckApproximations() {
    REQUIRE(Near(cbrt_nid_postfix(27.0), 3.0, 1e-14) && Near(cbrt_nid_postfix(-8.0), -2.0, 1e-14));
    REQUIRE(cbrt_nid_postfix(0.0) == 0.0 && cbrt_nid_postfix(Infinity) == Infinity);
    REQUIRE(Near(cbrtf_nid_postfix(125.0f), 5.0, 1e-6) && Near(cbrtf_nid_postfix(-1.0f), -1.0, 1e-6));
    REQUIRE(Near(log10_nid_postfix(1000.0), 3.0, 1e-14) && Near(log10_nid_postfix(0.01), -2.0, 1e-14));
    REQUIRE(Near(tan_nid_postfix(Pi / 4.0), 1.0, 1e-14));
    REQUIRE(Near(atan_nid_postfix(1.0), Pi / 4.0, 1e-15) && Near(atanf_nid_postfix(1.0f), Pi / 4.0, 1e-6));
    REQUIRE(Near(atan2_nid_postfix(1.0, 1.0), Pi / 4.0, 1e-15) && Near(atan2_nid_postfix(1.0, -1.0), 3.0 * Pi / 4.0, 1e-15));
    REQUIRE(Near(asin_nid_postfix(1.0), Pi / 2.0, 1e-15) && Near(acos_nid_postfix(-1.0), Pi, 1e-15));
    REQUIRE(Near(tanh_nid_postfix(0.5), 0.46211715726000974, 1e-14) && Near(tanhf_nid_postfix(0.5f), 0.46211715726000974, 1e-6));
    REQUIRE(Near(expf_nid_postfix(1.0f), 2.718281828459045, 1e-6) && Near(logf_nid_postfix(2.718281828f), 1.0, 1e-6));
    REQUIRE(Near(sinf_nid_postfix(1.0f), 0.8414709848078965, 1e-6) && Near(cosf_nid_postfix(1.0f), 0.5403023058681398, 1e-6));
    REQUIRE(Near(_FSinh_nid_postfix(1.0f, 2.0f), 2.0 * 1.1752011936438014, 1e-6));
    REQUIRE(Near(_FCosh_nid_postfix(1.0f, 2.0f), 2.0 * 1.5430806348152437, 1e-6));

    double sine = 0;
    double cosine = 0;
    sincos_nid_postfix(0.0, &sine, &cosine);
    REQUIRE(sine == 0.0 && cosine == 1.0);
    sincos_nid_postfix(Pi / 2.0, &sine, &cosine);
    REQUIRE(Near(sine, 1.0, 1e-15) && std::fabs(cosine) < 1e-15);
    sincos_nid_postfix(Pi, &sine, &cosine);
    REQUIRE(std::fabs(sine) < 1e-15 && Near(cosine, -1.0, 1e-15));
    float sineF = 0;
    float cosineF = 0;
    sincosf_nid_postfix(0.0f, &sineF, &cosineF);
    REQUIRE(sineF == 0.0f && cosineF == 1.0f);
    sincosf_nid_postfix(static_cast<float>(Pi / 2.0), &sineF, &cosineF);
    REQUIRE(Near(sineF, 1.0, 1e-6) && std::fabs(cosineF) < 1e-6);
}

}

int main() {
    CheckClassification();
    CheckExactArithmetic();
    CheckApproximations();
    std::puts("Guest math basics checks passed");
    return 0;
}
