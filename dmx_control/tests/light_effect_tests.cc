#include "dmx_control/light_effects.hh"
#include <assert.h>
#include <iostream>

//////////////////////////////////////////////
/// Helpers //////////////////////////////////
//////////////////////////////////////////////

inline bool colors_are_equal(const Color& lhs, const Color& rhs)
{
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}

struct test_fixture
{
    Color c0 {0, 0, 0};
    Color c1 {200, 100, 0};
};

static test_fixture fixture {};

//////////////////////////////////////////////
/// Tests ////////////////////////////////////
//////////////////////////////////////////////

void test_linear_fade(test_fixture f)
{
    constexpr uint8_t num_steps = 10;
    const auto linear = generate_transition<num_steps>(f.c0, f.c1);

    constexpr uint8_t r_first_step = 200 / num_steps;
    constexpr uint8_t g_first_step = 100 / num_steps;
    constexpr uint8_t b_first_step = 0 / num_steps;
    assert(colors_are_equal(linear.front(), {r_first_step, g_first_step, b_first_step}));

    assert(colors_are_equal(linear.back(), f.c1));
}

#define RUN_TEST(test_name) \
    std::cout << " [ RUNNING ]: " << #test_name << "\n"; \
    test_linear_fade(fixture); \
    std::cout << " [ PASSED  ]: " << #test_name << "\n"; \

int main()
{
    RUN_TEST(test_linear_fade);
}
