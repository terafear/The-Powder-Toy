#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_GRPH()
{
    Identifier = "DEFAULT_PT_GRPH";
    Name = "GRPH";
    Colour = 0x303030_rgb;
    MenuVisible = 1;
    MenuSection = SC_SOLIDS;
    Enabled = 1;

    Advection = 0.0f;
    AirDrag = 0.01f * CFDS;
    AirLoss = 0.95f;
    Loss = 0.00f;
    Collision = 0.0f;
    Gravity = 0.0f;
    Diffusion = 0.00f;
    HotAir = 0.000f * CFDS;
    Falldown = 1;

    Flammable = 0;
    Explosive = 0;
    Meltable = 1;
    Hardness = 85; 
    Weight = 100; 

    HeatConduct = 251; 
    Description = "Graphene. Extremely conductive, strong, and light.";

    Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_HOT_GLOW | PROP_LIFE_DEC;

    LowPressure = IPL;
    LowPressureTransition = NT;
    HighPressure = IPH;
    HighPressureTransition = NT;
    LowTemperature = ITL;
    LowTemperatureTransition = NT;
    HighTemperature = 3652.0f; // Melting point of Graphene
    HighTemperatureTransition = PT_LAVA;

    Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
    float temp = parts[i].temp;

    // Temperature equalization with neighboring particles
    for (int rx = -1; rx <= 1; rx++)
    {
        for (int ry = -1; ry <= 1; ry++)
        {
            if ((rx || ry) && (x + rx >= 0) && (y + ry >= 0) && (x + rx < XRES) && (y + ry < YRES))
            {
                int r = sim->pmap[y + ry][x + rx];
                if (!r)
                    continue;

                int rt = TYP(r);
                float rtemp = parts[ID(r)].temp;
                float temp_diff = temp - rtemp;

                // Thermal conductivity logic
                if (abs(temp_diff) > 1.0f)
                {
                    parts[i].temp -= temp_diff * 0.75f;
                    parts[ID(r)].temp += temp_diff * 0.75f;
                }
            }
        }
    }

    // Electrical conductivity logic
    if (temp > 3000.0f)
    {
        parts[i].ctype = PT_NONE;
    }
    else
    {
        parts[i].ctype = PT_SPRK;
    }

    // Glow effect based on temperature
    if (temp > 3000.0f)
    {
        parts[i].life = 4;
    }

    // Melting behavior
    if (temp >= 3652.0f)
    {
        sim->part_change_type(i, x, y, PT_LAVA);
    }

    return 0;
}
