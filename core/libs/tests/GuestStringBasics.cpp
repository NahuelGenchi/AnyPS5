#include "prx/libc/include/general/VabiMacros.hpp"
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

extern "C" {
void* APS5_VABI memmove_nid_postfix(void*, const void*, std::size_t);
const void* APS5_VABI memchr_nid_postfix(const void*, int, std::size_t);
void APS5_VABI bzero_nid_postfix(void*, std::size_t);
int APS5_VABI bcmp_nid_postfix(const void*, const void*, std::size_t);
char* APS5_VABI strcat_nid_postfix(char*, const char*);
char* APS5_VABI strncpy_nid_postfix(char*, const char*, std::size_t);
char* APS5_VABI strrchr_nid_postfix(const char*, int);
std::size_t APS5_VABI strspn_nid_postfix(const char*, const char*);
int APS5_VABI strcoll_nid_postfix(const char*, const char*);
std::size_t APS5_VABI strxfrm_nid_postfix(char*, const char*, std::size_t);
int APS5_VABI atoi_nid_postfix(const char*);
long long APS5_VABI strtoll_nid_postfix(const char*, char**, int);
unsigned long long APS5_VABI _Stoull_nid_postfix(const char*, char**, int);
double APS5_VABI strtod_nid_postfix(const char*, char**);
int APS5_VABI strncpy_s_nid_postfix(char*, std::size_t, const char*, std::size_t);
}

namespace {

void Require(bool condition, int line) {
    if (!condition) {
        std::fprintf(stderr, "Guest string basics check failed at line %d\n", line);
        std::abort();
    }
}

#define REQUIRE(condition) Require((condition), __LINE__)

void CheckMemoryFunctions() {
    char bytes[] = "abcdefgh";
    REQUIRE(memmove_nid_postfix(bytes + 2, bytes, 5) == bytes + 2 && std::strcmp(bytes, "ababcdeh") == 0);
    char backward[] = "abcdefgh";
    REQUIRE(memmove_nid_postfix(backward, backward + 2, 5) == backward && std::strcmp(backward, "cdefgfgh") == 0);
    char unchanged[] = "abc";
    REQUIRE(memmove_nid_postfix(unchanged, "xyz", 0) == unchanged && std::strcmp(unchanged, "abc") == 0);

    const char text[] = "hello";
    REQUIRE(memchr_nid_postfix(text, 'l', 5) == text + 2);
    REQUIRE(memchr_nid_postfix(text, 'z', 5) == nullptr);
    REQUIRE(memchr_nid_postfix(text, 'l', 2) == nullptr);
    REQUIRE(memchr_nid_postfix(text, 'l' + 256, 5) == text + 2);
    REQUIRE(memchr_nid_postfix(text, 0, 6) == text + 5);
    REQUIRE(memchr_nid_postfix(text, 'h', 0) == nullptr);

    unsigned char block[8];
    std::memset(block, 0xff, sizeof(block));
    bzero_nid_postfix(block + 2, 4);
    for (int index = 0; index < 8; ++index) REQUIRE(block[index] == (index >= 2 && index < 6 ? 0x00 : 0xff));
    bzero_nid_postfix(block, 0);
    REQUIRE(block[0] == 0xff && block[7] == 0xff);

    REQUIRE(bcmp_nid_postfix("abcd", "abcd", 4) == 0);
    REQUIRE(bcmp_nid_postfix("abcd", "abce", 4) != 0);
    REQUIRE(bcmp_nid_postfix("abcd", "abce", 3) == 0);
    REQUIRE(bcmp_nid_postfix("abc", "xyz", 0) == 0);
}

void CheckCopyAndSearch() {
    char joined[16] = "ab";
    REQUIRE(strcat_nid_postfix(joined, "cd") == joined && std::strcmp(joined, "abcd") == 0);
    REQUIRE(strcat_nid_postfix(joined, "") == joined && std::strcmp(joined, "abcd") == 0);
    char empty[8] = "";
    REQUIRE(strcat_nid_postfix(empty, "xy") == empty && std::strcmp(empty, "xy") == 0);

    char padded[8];
    std::memset(padded, 'x', sizeof(padded));
    REQUIRE(strncpy_nid_postfix(padded, "ab", 5) == padded);
    REQUIRE(padded[0] == 'a' && padded[1] == 'b' && padded[2] == 0 && padded[3] == 0 && padded[4] == 0 && padded[5] == 'x');
    std::memset(padded, 'x', sizeof(padded));
    strncpy_nid_postfix(padded, "abcdef", 3);
    REQUIRE(padded[0] == 'a' && padded[1] == 'b' && padded[2] == 'c' && padded[3] == 'x');
    std::memset(padded, 'x', sizeof(padded));
    strncpy_nid_postfix(padded, "abc", 0);
    REQUIRE(padded[0] == 'x');

    const char path[] = "a/b/c";
    REQUIRE(strrchr_nid_postfix(path, '/') == path + 3);
    REQUIRE(strrchr_nid_postfix(path, 'a') == path);
    REQUIRE(strrchr_nid_postfix(path, 'z') == nullptr);
    REQUIRE(strrchr_nid_postfix(path, 0) == path + 5);
    REQUIRE(strrchr_nid_postfix("", 0) != nullptr);

    REQUIRE(strspn_nid_postfix("abcabcd", "abc") == 6);
    REQUIRE(strspn_nid_postfix("xyz", "abc") == 0);
    REQUIRE(strspn_nid_postfix("", "abc") == 0);
    REQUIRE(strspn_nid_postfix("abc", "") == 0);
    REQUIRE(strspn_nid_postfix("abc", "abc") == 3);
}

void CheckCollation() {
    REQUIRE(strcoll_nid_postfix("a", "b") < 0);
    REQUIRE(strcoll_nid_postfix("b", "a") > 0);
    REQUIRE(strcoll_nid_postfix("a", "a") == 0);
    REQUIRE(strcoll_nid_postfix("", "a") < 0);
    REQUIRE(strcoll_nid_postfix("\xff", "a") > 0);

    char transformed[16];
    REQUIRE(strxfrm_nid_postfix(transformed, "abc", sizeof(transformed)) == 3 && std::strcmp(transformed, "abc") == 0);
    REQUIRE(strxfrm_nid_postfix(nullptr, "abc", 0) == 3);
    REQUIRE(strxfrm_nid_postfix(transformed, "abcdef", 4) == 6);
}

void CheckNumbers() {
    REQUIRE(atoi_nid_postfix("42") == 42);
    REQUIRE(atoi_nid_postfix("  -17xyz") == -17);
    REQUIRE(atoi_nid_postfix("+5") == 5);
    REQUIRE(atoi_nid_postfix("0012") == 12);
    REQUIRE(atoi_nid_postfix("abc") == 0);
    REQUIRE(atoi_nid_postfix("") == 0);

    char* end = nullptr;
    const char* text = "123abc";
    REQUIRE(strtoll_nid_postfix(text, &end, 10) == 123 && end == text + 3);
    text = "-0x1F";
    REQUIRE(strtoll_nid_postfix(text, &end, 16) == -31 && end == text + 5);
    text = "0x1f";
    REQUIRE(strtoll_nid_postfix(text, &end, 0) == 31 && end == text + 4);
    text = "017";
    REQUIRE(strtoll_nid_postfix(text, &end, 0) == 15 && end == text + 3);
    REQUIRE(strtoll_nid_postfix(text, &end, 10) == 17 && end == text + 3);
    text = "zz";
    REQUIRE(strtoll_nid_postfix(text, &end, 36) == 35 * 36 + 35 && end == text + 2);
    text = "   42";
    REQUIRE(strtoll_nid_postfix(text, &end, 10) == 42 && end == text + 5);
    text = "abc";
    REQUIRE(strtoll_nid_postfix(text, &end, 10) == 0 && end == text);
    REQUIRE(strtoll_nid_postfix("9223372036854775807", nullptr, 10) == LLONG_MAX);
    REQUIRE(strtoll_nid_postfix("9223372036854775808", nullptr, 10) == LLONG_MAX);
    REQUIRE(strtoll_nid_postfix("-9223372036854775808", nullptr, 10) == LLONG_MIN);
    REQUIRE(strtoll_nid_postfix("-9223372036854775809", nullptr, 10) == LLONG_MIN);

    text = "18446744073709551615";
    REQUIRE(_Stoull_nid_postfix(text, &end, 10) == ULLONG_MAX && end == text + 20);
    text = "0xff";
    REQUIRE(_Stoull_nid_postfix(text, &end, 0) == 255 && end == text + 4);
    REQUIRE(_Stoull_nid_postfix("-1", nullptr, 10) == ULLONG_MAX);
    REQUIRE(_Stoull_nid_postfix("18446744073709551616", nullptr, 10) == ULLONG_MAX);
    text = "xyz";
    REQUIRE(_Stoull_nid_postfix(text, &end, 10) == 0 && end == text);

    text = "  3.5xyz";
    REQUIRE(strtod_nid_postfix(text, &end) == 3.5 && end == text + 5);
    text = "-1.25e2";
    REQUIRE(strtod_nid_postfix(text, &end) == -125.0 && end == text + 7);
    text = ".5";
    REQUIRE(strtod_nid_postfix(text, &end) == 0.5 && end == text + 2);
    text = "abc";
    REQUIRE(strtod_nid_postfix(text, &end) == 0.0 && end == text);
    REQUIRE(std::isinf(strtod_nid_postfix("inf", nullptr)) && strtod_nid_postfix("inf", nullptr) > 0);
    REQUIRE(std::isinf(strtod_nid_postfix("-inf", nullptr)) && strtod_nid_postfix("-inf", nullptr) < 0);
    REQUIRE(std::isinf(strtod_nid_postfix("1e999", nullptr)));
    REQUIRE(std::fabs(strtod_nid_postfix("1e-999", nullptr)) < 1e-300);
}

void CheckBoundedCopy() {
    char destination[8];
    std::memset(destination, 'x', sizeof(destination));
    REQUIRE(strncpy_s_nid_postfix(destination, sizeof(destination), "abcdef", 6) == 0 && std::strcmp(destination, "abcdef") == 0);
    REQUIRE(strncpy_s_nid_postfix(destination, sizeof(destination), "abcdef", 2) == 0 && std::strcmp(destination, "ab") == 0);
    REQUIRE(strncpy_s_nid_postfix(destination, sizeof(destination), "abcdefg", 7) == 0 && std::strcmp(destination, "abcdefg") == 0);
    REQUIRE(strncpy_s_nid_postfix(destination, sizeof(destination), "abcdefgh", 8) == 34 && destination[0] == '\0');
    REQUIRE(strncpy_s_nid_postfix(destination, sizeof(destination), nullptr, 4) == 22 && destination[0] == '\0');
    REQUIRE(strncpy_s_nid_postfix(nullptr, sizeof(destination), "a", 1) == 22);
    REQUIRE(strncpy_s_nid_postfix(destination, 0, "a", 1) == 22);
}

}

int main() {
    CheckMemoryFunctions();
    CheckCopyAndSearch();
    CheckCollation();
    CheckNumbers();
    CheckBoundedCopy();
    std::puts("Guest string basics checks passed");
    return 0;
}
