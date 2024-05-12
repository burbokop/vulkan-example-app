#include "resources.h"

e172vp::Mesh Resources::mesh(const std::string& resource)
{
    if(resource == "plate") {
        return e172vp::Mesh({ { { -0.5f, -0.5f, 0 }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
                                { { 0.5f, -0.5f, 0 }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
                                { { 0.5f, 0.5f, 0 }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
                                { { -0.5f, 0.5f, 0 }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } } },
            { 0, 1, 2,
                2, 3, 0 });
    }

    return e172vp::Mesh({ { { 4.5f, -0.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { 4.5f, 0.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { 0.5f, 1.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { 0.5f, 0.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { 0.5f, -0.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { 0.5f, -1.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { -2.5f, -5.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { -4.5f, -5.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { -2.5f, -1.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { -2.5f, -0.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { -3.5f, -0.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { -4.5f, -0.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { -4.5f, 0.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { -3.5f, 0.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { -2.5f, 0.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { -2.5f, 1.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { -4.5f, 5.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} },
                            { { -2.5f, 5.5f, 0 }, { 1.0f, 0.0f, 0.0f }, {} } },
        { 3, 0, 1,
            1, 2, 3,
            3, 4, 0,
            0, 4, 5,
            5, 8, 7,
            7, 6, 5,
            8, 9, 10,
            10, 12, 11,
            9, 12, 10,
            9, 12, 13,
            9, 14, 12,
            9, 13, 14,
            13, 14, 15,
            15, 8, 5,
            15, 5, 2,
            15, 2, 16,
            16, 2, 17 });
}
