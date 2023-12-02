#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_GRPT()
{
    Identifier = "DEFAULT_PT_GRPT";
    Name = "GRPT";
    Colour = 0x404040_rgb; 
    MenuVisible = 1;
    MenuSection = SC_SOLIDS;
    Enabled = 1;

    Advection = 0.0f;
    AirDrag = 0.02f * CFDS;
    AirLoss = 0.92f;
    Loss = 0.00f;
    Collision = 0.0f;
    Gravity = 0.1f;
    Diffusion = 0.00f;
    HotAir = 0.000f * CFDS;
    Falldown = 2;

    Flammable = 0; 
    Explosive = 0;
    Meltable = 1;
    Hardness = 30; 
    Weight = 100; 

    HeatConduct = 100; 
    Description = "Graphite. Good conductor of electricity, moderate thermal conductor.";

    Properties = TYPE_SOLID |PROP_CONDUCTS | PROP_HOT_GLOW | PROP_LIFE_DEC;

    LowPressure = IPL;
    LowPressureTransition = NT;
    HighPressure = IPH;
    HighPressureTransition = NT;
    LowTemperature = ITL;
    LowTemperatureTransition = NT;
    HighTemperature = 3800.0f; 
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

                // Thermal conductivity logic for graphite
                if (abs(temp_diff) > 1.0f)
                {
                    parts[i].temp -= temp_diff * 0.25f;
                    parts[ID(r)].temp += temp_diff * 0.25f;
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
    if (temp > 3500.0f)
    {
        parts[i].life = 4;
    }
    else
    {
        parts[i].life = 0;
    }

    // Melting behavior
    if (temp >= 3800.0f) 
    {
        sim->part_change_type(i, x, y, PT_LAVA);
    }

    return 0;
}
