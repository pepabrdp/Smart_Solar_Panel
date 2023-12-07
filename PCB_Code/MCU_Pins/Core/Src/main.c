#include "main.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C3_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART1_UART_Init(void);

///////////////////////////////////////////////////////////////////////////////
//FUNCTIONS

//SOLAR ALGORITHM FUNCTIONS
float calculateAngleAdjustment(float flux1, float flux2);
float decideAdjustment(float fluxSensor1, float fluxSensor2);
void lightFollowOnlyMode();

double getDayinDecimals(int, double, double, double);
double getJulianDay(int, int, double, double);
double getJulianEphemerisDay(double);
double getJulianCentury(double);
double getJulianEphemerisCentury(double);

struct L0 *initL0();
struct L1 *initL1();
struct L2 *initL2();
struct L3 *initL3();
struct L4 *initL4();
struct L5 *initL5();
double getL0(struct L0 *, double);
double getL1(struct L1 *, double);
double getL2(struct L2 *, double);
double getL3(struct L3 *, double);
double getL4(struct L4 *, double);
double getL5(struct L5 *, double);
double getHeliocentricLongitude(double, double, double, double, double, double, double);

struct B0 *initB0();
struct B1 *initB1();
double getB0(struct B0 *, double);
double getB1(struct B1 *, double);
double getHeliocentricLatitude(double, double, double);

struct R0 *initR0();
struct R1 *initR1();
struct R2 *initR2();
struct R3 *initR3();
struct R4 *initR4();
double getR0(struct R0 *, double);
double getR1(struct R1 *, double);
double getR2(struct R2 *, double);
double getR3(struct R3 *, double);
double getR4(struct R4 *, double);
double getEarthRadiusVector(double, double, double, double, double, double);

double getGeocentricLongitude(double);
double getMeanElongation_MoonFromSun(double);
double getMeanAnomalySun(double);
double getMeanAnomalyMoon(double);
double getMoonLatitudeArgument(double);
double getMoonAscendingNode(double);

struct L0
{
    long a;
    double b;
    double c;
};

struct L1
{
    long a;
    double b;
    double c;
};

struct L2
{
    long a;
    double b;
    double c;
};

struct L3
{
    long a;
    double b;
    double c;
};

struct L4
{
    long a;
    double b;
    double c;
};

struct L5
{
    long a;
    double b;
    double c;
};

struct B0
{
    long a;
    double b;
    double c;
};

struct B1
{
    long a;
    double b;
    double c;
};

struct R0
{
    long a;
    double b;
    double c;
};

struct R1
{
    long a;
    double b;
    double c;
};

struct R2
{
    long a;
    double b;
    double c;
};

struct R3
{
    long a;
    double b;
    double c;
};

struct R4
{
    long a;
    double b;
    double c;
};


double getDayinDecimals(int day, double hour, double min, double sec)
{
    double hour_in_decimal = hour / 24;
    double min_in_decimal = min / (60 * 24);
    double sec_in_decimal = sec / (60 * 60 * 24);

    double day_in_decimal = day + hour_in_decimal + min_in_decimal + sec_in_decimal;

    return day_in_decimal;
}

double getJulianDay(int year, int month, double day, double zone)
{
    if (month == 1 || month == 2)
    {
        month += 12;
        year -= 1;
    }

    int a = (int)(year / 100);
    int b = 2 - a + (int)(a / 4);

    double julianDay = (int)(365.25 * (year + 4716)) + (int)(30.6001 * (month + 1)) + day - (zone / 24) + b - 1524.5;

    return julianDay;
}

double getJulianEphemerisDay(double julianDay)
{
    double julianEphemerisDay = julianDay + (69 / 86400);
    return julianEphemerisDay;
}

double getJulianCentury(double julianDay)
{
    double julianCentury = (julianDay - 2451545) / 36525;
    return julianCentury;
}

double getJulianEphemerisCentury(double julianEphemerisDay)
{
    double julianEphemerisCentury = (julianEphemerisDay - 2451545) / 36525;
    return julianEphemerisCentury;
}

double getJulianEphemerisMillenium(double julianEphemerisCentury)
{
    double julianEphemerisMillenium = julianEphemerisCentury / 10;
    return julianEphemerisMillenium;
}


struct L0 *initL0()
{
    static struct L0 data[64] = {
        {175347046, 0, 0},
        {3341656, 4.6692568, 6283.07585},
        {34894, 4.6261, 12566.1517},
        {3497, 2.7441, 5753.3849},
        {3418, 2.8289, 3.5231},
        {3136, 3.6277, 77713.7715},
        {2676, 4.4181, 7860.4194},
        {2343, 6.1352, 3930.2097},
        {1324, 0.7425, 11506.7698},
        {1273, 2.0371, 529.691},
        {1199, 1.1096, 1577.3435},
        {990, 5.233, 5884.927},
        {902, 2.045, 26.298},
        {857, 3.508, 398.149},
        {780, 1.179, 5223.694},
        {753, 2.533, 5507.553},
        {505, 4.583, 18849.228},
        {492, 4.205, 775.523},
        {357, 2.92, 0.067},
        {317, 5.849, 11790.629},
        {284, 1.899, 796.298},
        {271, 0.315, 10977.079},
        {243, 0.345, 5486.778},
        {206, 4.806, 2544.314},
        {205, 1.869, 5573.143},
        {202, 2.458, 6069.777},
        {156, 0.833, 213.299},
        {132, 3.411, 2942.463},
        {126, 1.083, 20.775},
        {115, 0.645, 0.98},
        {103, 0.636, 4694.003},
        {102, 0.976, 15720.839},
        {102, 4.267, 7.114},
        {99, 6.21, 2146.17},
        {98, 0.68, 155.42},
        {86, 5.98, 161000.69},
        {85, 1.3, 6275.96},
        {85, 3.67, 71430.7},
        {80, 1.81, 17260.15},
        {79, 3.04, 12036.46},
        {75, 1.76, 5088.63},
        {74, 3.5, 3154.69},
        {74, 4.68, 801.82},
        {70, 0.83, 9437.76},
        {62, 3.98, 8827.39},
        {61, 1.82, 7084.9},
        {57, 2.78, 6286.6},
        {56, 4.39, 14143.5},
        {56, 3.47, 6279.55},
        {52, 0.19, 12139.55},
        {52, 1.33, 1748.02},
        {51, 0.28, 5856.48},
        {49, 0.49, 1194.45},
        {41, 5.37, 8429.24},
        {41, 2.4, 19651.05},
        {39, 6.17, 10447.39},
        {37, 6.04, 10213.29},
        {37, 2.57, 1059.38},
        {36, 1.71, 2352.87},
        {36, 1.78, 6812.77},
        {33, 0.59, 17789.85},
        {30, 0.44, 83996.85},
        {30, 2.74, 1349.87},
        {25, 3.16, 4690.48}
    };

    return data;
}

struct L1 *initL1()
{
    static struct L1 data[34] = {
        {628331966747, 0, 0},
        {206059, 2.678235, 6283.07585},
        {4303, 2.6351, 12566.1517},
        {425, 1.59, 3.523},
        {119, 5.796, 26.298},
        {109, 2.966, 1577.344},
        {93, 2.59, 18849.23},
        {72, 1.14, 529.69},
        {68, 1.87, 398.15},
        {67, 4.41, 5507.55},
        {59, 2.89, 5223.69},
        {56, 2.17, 155.42},
        {45, 0.4, 796.3},
        {36, 0.47, 775.52},
        {29, 2.65, 7.11},
        {21, 5.34, 0.98},
        {19, 1.85, 5486.78},
        {19, 4.97, 213.3},
        {17, 2.99, 6275.96},
        {16, 0.03, 2544.31},
        {16, 1.43, 2146.17},
        {15, 1.21, 10977.08},
        {12, 2.83, 1748.02},
        {12, 3.26, 5088.63},
        {12, 5.27, 1194.45},
        {12, 2.08, 4694},
        {11, 0.77, 553.57},
        {10, 1.3, 6286.6},
        {10, 4.24, 1349.87},
        {9, 2.7, 242.73},
        {9, 5.64, 951.72},
        {8, 5.3, 2352.87},
        {6, 2.65, 9437.76},
        {6, 4.67, 4690.48}
    };

    return data;
}


struct L2 *initL2()
{
    static struct L2 data[20] = {
        {52919, 0, 0},
        {8720, 1.0721, 6283.0758},
        {309, 0.867, 12566.152},
        {27, 0.05, 3.52},
        {16, 5.19, 26.3},
        {16, 3.68, 155.42},
        {10, 0.76, 18849.23},
        {9, 2.06, 77713.77},
        {7, 0.83, 775.52},
        {5, 4.66, 1577.34},
        {4, 1.03, 7.11},
        {4, 3.44, 5573.14},
        {3, 5.14, 796.3},
        {3, 6.05, 5507.55},
        {3, 1.19, 242.73},
        {3, 6.12, 529.69},
        {3, 0.31, 398.15},
        {3, 2.28, 553.57},
        {2, 4.38, 5223.69},
        {2, 3.75, 0.98}
    };

    return data;
}

struct L3 *initL3()
{
    static struct L3 data[7] = {
        {289, 5.844, 6283.076},
        {35, 0, 0},
        {17, 5.49, 12566.15},
        {3, 5.2, 155.42},
        {1, 4.72, 3.52},
        {1, 5.3, 18849.23},
        {1, 5.97, 242.73}
    };

    return data;
}

struct L4 *initL4()
{
    static struct L4 data[3] = {
        {114, 3.142, 0},
        {8, 4.13, 6283.08},
        {1, 3.84, 12566.15}
    };

    return data;
}

struct L5 *initL5()
{
    static struct L5 data[1] = {
        {1, 3.14, 0}
    };

    return data;
}

double getL0(struct L0 *data, double julianEphemerisMillenium)
{
    double L0 = 0;
    for (int i = 0; i < 64; i++)
    {
        L0 += ((data + i) -> a) * cos(((data + i) -> b) + (((data + i) -> c) * julianEphemerisMillenium));
    }
    return L0;
}

double getL1(struct L1 *data, double julianEphemerisMillenium)
{
    double L1 = 0;
    for (int i = 0; i < 34; i++)
    {
        L1 += ((data + i) -> a) * cos(((data + i) -> b) + (((data + i) -> c) * julianEphemerisMillenium));
    }
    return L1;
}

double getL2(struct L2 *data, double julianEphemerisMillenium)
{
    double L2 = 0;
    for (int i = 0; i < 20; i++)
    {
        L2 += ((data + i) -> a) * cos(((data + i) -> b) + (((data + i) -> c) * julianEphemerisMillenium));
    }
    return L2;
}

double getL3(struct L3 *data, double julianEphemerisMillenium)
{
    double L3 = 0;
    for (int i = 0; i < 7; i++)
    {
        L3 += ((data + i) -> a) * cos(((data + i) -> b) + (((data + i) -> c) * julianEphemerisMillenium));
    }
    return L3;
}

double getL4(struct L4 *data, double julianEphemerisMillenium)
{
    double L4 = 0;
    for (int i = 0; i < 3; i++)
    {
        L4 += ((data + i) -> a) * cos(((data + i) -> b) + (((data + i) -> c) * julianEphemerisMillenium));
    }
    return L4;
}

double getL5(struct L5 *data, double julianEphemerisMillenium)
{
    double L5 = 0;
    for (int i = 0; i < 1; i++)
    {
        L5 += ((data + i) -> a) * cos(((data + i) -> b) + (((data + i) -> c) * julianEphemerisMillenium));
    }
    return L5;
}

double getHeliocentricLongitude(double L0, double L1, double L2, double L3, double L4, double L5, double JME)
{
    double heliocentric_longitude = (L0 + (L1 * JME) + (L2 * pow(JME, 2)) + (L3 * pow(JME, 3)) + (L4 * pow(JME, 4)) + (L5 * pow(JME, 5))) / pow(10, 8);

    heliocentric_longitude = (heliocentric_longitude * 180) / M_PI;
    heliocentric_longitude = heliocentric_longitude / 360;

    double fraction = fabs(heliocentric_longitude - floor(fabs(heliocentric_longitude)));

    if (heliocentric_longitude > 0)
    {
        heliocentric_longitude = 360 * fraction;
    }
    else
    {
        heliocentric_longitude = 360 - (360 * fraction);
    }

    return heliocentric_longitude;
}

struct B0 *initB0()
{
    static struct B0 data[5] = {
        {280, 3.199, 84334.662},
        {102, 5.422, 5507.553},
        {80, 3.88, 5223.69},
        {44, 3.7, 2352.87},
        {32, 4, 1577.34}
    };

    return data;
}

struct B1 *initB1()
{
    static struct B1 data[2] = {
        {9, 3.9, 5507.55},
        {6, 1.73, 5223.69}
    };

    return data;
}

double getB0(struct B0 *data, double julianEphemerisMillenium)
{
    double B0 = 0;
    for (int i = 0; i < 5; i++)
    {
        B0 += ((data + i) -> a) * cos(((data + i) -> b) + (((data + i) -> c) * julianEphemerisMillenium));
    }
    return B0;
}

double getB1(struct B1 *data, double julianEphemerisMillenium)
{
    double B1 = 0;
    for (int i = 0; i < 2; i++)
    {
        B1 += ((data + i) -> a) * cos(((data + i) -> b) + (((data + i) -> c) * julianEphemerisMillenium));
    }
    return B1;
}

double getHeliocentricLatitude(double B0, double B1, double JME)
{
    double heliocentric_latitude = (B0 + (B1 * JME)) / pow(10, 8);

    heliocentric_latitude = (heliocentric_latitude * 180) / M_PI;

    return heliocentric_latitude;
}


struct R0 *initR0()
{
    static struct R0 data[40] = {
        {100013989, 0, 0},
        {1670700, 3.0984635, 6283.07585},
        {13956, 3.05525, 12566.1517},
        {3084, 5.1985, 77713.7715},
        {1628, 1.1739, 5753.3849},
        {1576, 2.8469, 7860.4194},
        {925, 5.453, 11506.77},
        {542, 4.564, 3930.21},
        {472, 3.661, 5884.927},
        {346, 0.964, 5507.553},
        {329, 5.9, 5223.694},
        {307, 0.299, 5573.143},
        {243, 4.273, 11790.629},
        {212, 5.847, 1577.344},
        {186, 5.022, 10977.079},
        {175, 3.012, 18849.228},
        {110, 5.055, 5486.778},
        {98, 0.89, 6069.78},
        {86, 5.69, 15720.84},
        {86, 1.27, 161000.69},
        {65, 0.27, 17260.15},
        {63, 0.92, 529.69},
        {57, 2.01, 83996.85},
        {56, 5.24, 71430.7},
        {49, 3.25, 2544.31},
        {47, 2.58, 775.52},
        {45, 5.54, 9437.76},
        {43, 6.01, 6275.96},
        {39, 5.36, 4694},
        {38, 2.39, 8827.39},
        {37, 0.83, 19651.05},
        {37, 4.9, 12139.55},
        {36, 1.67, 12036.46},
        {35, 1.84, 2942.46},
        {33, 0.24, 7084.9},
        {32, 0.18, 5088.63},
        {32, 1.78, 398.15},
        {28, 1.21, 6286.6},
        {28, 1.9, 6279.55},
        {26, 4.59, 10447.39}
    };

    return data;
}


struct R1 *initR1()
{
    static struct R1 data[10] = {
        {103019, 1.10749, 6283.07585},
        {1721, 1.0644, 12566.1517},
        {702, 3.142, 0},
        {32, 1.02, 18849.23},
        {31, 2.84, 5507.55},
        {25, 1.32, 5223.69},
        {18, 1.42, 1577.34},
        {10, 5.91, 10977.08},
        {9, 1.42, 6275.96},
        {9, 0.27, 5486.78}
    };

    return data;
}

struct R2 *initR2()
{
    static struct R2 data[6] = {
        {4359, 5.7846, 6283.0758},
        {124, 5.579, 12566.152},
        {12, 3.14, 0},
        {9, 3.63, 77713.77},
        {6, 1.87, 5573.14},
        {3, 5.47, 18849.23}
    };

    return data;
}


struct R3 *initR3()
{
    static struct R3 data[2] = {
        {145, 4.273, 6283.076},
        {7, 3.92, 12566.15}
    };

    return data;
}

struct R4 *initR4()
{
    static struct R4 data[1] = {
        {4, 2.56, 6283.08}
    };

    return data;
}

double getR0(struct R0 *data, double julianEphemerisMillenium)
{
    double R0 = 0;
    for (int i = 0; i < 40; i++)
    {
        R0 += ((data + i) -> a) * cos(((data + i) -> b) + (((data + i) -> c) * julianEphemerisMillenium));
    }
    return R0;
}

double getR1(struct R1 *data, double julianEphemerisMillenium)
{
    double R1 = 0;
    for (int i = 0; i < 10; i++)
    {
        R1 += ((data + i) -> a) * cos(((data + i) -> b) + (((data + i) -> c) * julianEphemerisMillenium));
    }
    return R1;
}

double getR2(struct R2 *data, double julianEphemerisMillenium)
{
    double R2 = 0;
    for (int i = 0; i < 6; i++)
    {
        R2 += ((data + i) -> a) * cos(((data + i) -> b) + (((data + i) -> c) * julianEphemerisMillenium));
    }
    return R2;
}

double getR3(struct R3 *data, double julianEphemerisMillenium)
{
    double R3 = 0;
    for (int i = 0; i < 2; i++)
    {
        R3 += ((data + i) -> a) * cos(((data + i) -> b) + (((data + i) -> c) * julianEphemerisMillenium));
    }
    return R3;
}

double getR4(struct R4 *data, double julianEphemerisMillenium)
{
    double R4 = 0;
    for (int i = 0; i < 1; i++)
    {
        R4 += ((data + i) -> a) * cos(((data + i) -> b) + (((data + i) -> c) * julianEphemerisMillenium));
    }
    return R4;
}

double getEarthRadiusVector(double R0, double R1, double R2, double R3, double R4, double JME)
{
    double earth_radiusVector = (R0 + (R1 * JME) + (R2 * pow(JME, 2)) + (R3 * pow(JME, 3)) + (R4 * pow(JME, 4))) / pow(10, 8);

    return earth_radiusVector;
}

double getGeocentricLongitude(double heliocentric_longitude)
{
    double geocentric_longitude = heliocentric_longitude + 180;
    geocentric_longitude = geocentric_longitude / 360;

    double fraction = fabs(geocentric_longitude - floor(fabs(geocentric_longitude)));

    if (geocentric_longitude > 0)
    {
        geocentric_longitude = 360 * fraction;
    }
    else
    {
        geocentric_longitude = 360 - (360 * fraction);
    }

    return geocentric_longitude;
}

double getMeanElongation_MoonFromSun(double julianEphemerisCentury)
{
    double mean_elongation = 297.85036 + (445267.111480 * julianEphemerisCentury) - (0.0019142 * pow(julianEphemerisCentury, 2)) +
    (pow(julianEphemerisCentury, 3) / 189474);

    return mean_elongation;
}

double getMeanAnomalySun(double julianEphemerisCentury)
{
    double mean_anomaly = 357.52772 + (35999.050340 * julianEphemerisCentury) - (0.0001603 * pow(julianEphemerisCentury, 2)) -
    (pow(julianEphemerisCentury, 3) / 300000);

    return mean_anomaly;
}

double getMeanAnomalyMoon(double julianEphemerisCentury)
{
    double mean_anomaly = 134.96298 + (477198.867398 * julianEphemerisCentury) + (0.0086972 * pow(julianEphemerisCentury, 2)) +
    (pow(julianEphemerisCentury, 3) / 56250);

    return mean_anomaly;
}

double getMoonLatitudeArgument(double julianEphemerisCentury)
{
    double moon_latitude_argument = 93.27191 + (483202.017538 * julianEphemerisCentury) + (0.0036825 * pow(julianEphemerisCentury, 2)) +
    (pow(julianEphemerisCentury, 3) / 327270);

    return moon_latitude_argument;
}

double getMoonAscendingNode(double julianEphemerisCentury)
{
    double moon_ascending_node = 125.04452 - (1934.136261 * julianEphemerisCentury) + (0.0020708 * pow(julianEphemerisCentury, 2)) +
    (pow(julianEphemerisCentury, 3) / 450000);

    return moon_ascending_node;
}

double *getNutation(const int Y[63][5], const double abcd[63][4], double JCE, double x0, double x1, double x2, double x3, double x4)
{
    double phi = 0;
    double eps = 0;
    for (int i = 0; i < 63; i++)
    {
        double xySum = x0*Y[i][0] + x1*Y[i][1] + x2*Y[i][2] + x3*Y[i][3] + x4*Y[i][4];
        xySum = (M_PI/180.0) * xySum;
        phi += (abcd[i][0] + (abcd[i][1] * JCE)) * sin(xySum);
        eps += (abcd[i][2] + (abcd[i][3] * JCE)) * cos(xySum);
    }
    phi = phi / 36000000;
    eps = eps / 36000000;

    double *nutation = malloc(2 * sizeof(double));
    nutation[0] = phi;
    nutation[1] = eps;

    return nutation;
}

double getMeanObliquity(double U)
{
    double E = 84381.448 - (4680.93 * U) - (1.55 * pow(U, 2)) + (1999.25 * pow(U, 3)) - (51.38 * pow(U, 4)) - (249.67 * pow(U, 5)) -
    (39.05 * pow(U, 6)) + (7.12 * pow(U, 7)) + (27.87 * pow(U, 8)) + (5.79 * pow(U, 9)) + (2.45 * pow(U, 10));

    return E;
}

double getApparentSiderealTime(double JD, double JC, double phi, double eps)
{
    double mean_siderealTime = 280.46061837 + (360.98564736629 * (JD - 2451545)) + (0.000387933 * pow(JC, 2)) - (pow(JC, 3) / 38710000);

    mean_siderealTime = mean_siderealTime / 360;
    double fraction = fabs(mean_siderealTime - floor(fabs(mean_siderealTime)));
    if (mean_siderealTime > 0)
    {
        mean_siderealTime = 360 * fraction;
    }
    else
    {
        mean_siderealTime = 360 - (360 * fraction);
    }

    eps = (M_PI / 180.0) * eps;
    double apparent_siderealTime = mean_siderealTime + (phi * cos(eps));

    return apparent_siderealTime;
}

double getSunRightAscension(double lambda, double eps, double beta)
{
    lambda = (M_PI / 180.0) * lambda;
    eps = (M_PI / 180.0) * eps;
    beta = (M_PI / 180.0) * beta;

    double alpha = atan2((sin(lambda)*cos(eps)) - (tan(beta)*sin(eps)), cos(lambda));
    alpha = (alpha * 180) / M_PI;

    alpha = alpha / 360;
    double fraction = fabs(alpha - floor(fabs(alpha)));
    if (alpha > 0)
    {
        alpha = 360 * fraction;
    }
    else
    {
        alpha = 360 - (360 * fraction);
    }

    return alpha;
}


double getSunDeclination(double lambda, double eps, double beta)
{
    lambda = (M_PI / 180.0) * lambda;
    eps = (M_PI / 180.0) * eps;
    beta = (M_PI / 180.0) * beta;

    double delta = asin((sin(beta)*cos(eps)) + (cos(beta)*sin(eps)*sin(lambda)));
    delta = (delta * 180) / M_PI;

    return delta;
}

double getObserverLocalHourAngle(double v, double sigma, double alpha)
{
    double H = v + sigma - alpha;
    H = H / 360;
    double fraction = fabs(H - floor(fabs(H)));
    if (H > 0)
    {
        H = 360 * fraction;
    }
    else
    {
        H = 360 - (360 * fraction);
    }

    return H;
}

double getEquatorialHorizontalParallax(double R)
{
    double parallax = 8.794 / (3600 * R);

    return parallax;
}

double helper_getU(double latitude)
{
    double u = atan(0.99664719 * tan(latitude));

    return u;
}

double helper_getX(double u, double E, double latitude)
{
    u = (M_PI / 180) * u;
    latitude = (M_PI /  180) * latitude;

    double x = cos(u) + (E / 6378140)*cos(latitude);

    return x;
}

double helper_getY(double u, double E, double latitude)
{
    u = (M_PI / 180) * u;
    latitude = (M_PI /  180) * latitude;

    double y = 0.99664719*sin(u) + (E / 6378140)*sin(latitude);

    return y;
}

double getParallax_SunRightAscension(double x, double eqHorizontal_parallax, double H, double delta)
{
    x = (M_PI / 180) * x;
    eqHorizontal_parallax = (M_PI / 180) * eqHorizontal_parallax;
    H = (M_PI / 180) * H;
    delta = (M_PI / 180) * delta;

    double term1 = -x * sin(eqHorizontal_parallax) * sin(H);
    double term2 = cos(delta) - (x * sin(eqHorizontal_parallax) * cos(H));

    double parallax = atan2(term1, term2);
    parallax = (parallax * 180) / M_PI;

    return parallax;
}

double getTopocentricSunRightAscension(double alpha, double parallax)
{
    double alpha_dash = alpha + parallax;

    return alpha_dash;
}

double getTopocentricSunDeclination(double delta, double y, double x, double eqHorizontal_parallax, double delta_alpha, double H)
{
    x = (M_PI / 180) * x;
    y = (M_PI / 180) * y;
    eqHorizontal_parallax = (M_PI / 180) * eqHorizontal_parallax;
    H = (M_PI / 180) * H;
    delta = (M_PI / 180) * delta;
    delta_alpha = (M_PI / 180) * delta_alpha;

    double term1 = (sin(delta) - (y*sin(eqHorizontal_parallax))) * cos(delta_alpha);
    double term2 = cos(delta) - (x * sin(eqHorizontal_parallax) * cos(H));

    double delta_dash = atan2(term1, term2);
    delta_dash = (delta_dash * 180) / M_PI;

    return delta_dash;
}

double getTopocentricLocalHourAngle(double H, double alpha_delta)
{
    double H_dash = H - alpha_delta;

    return H_dash;
}

double getIncompleteElevationAngle(double latitude, double delta_dash, double H_dash)
{
    latitude = (M_PI / 180) * latitude;
    delta_dash = (M_PI / 180) * delta_dash;
    H_dash = (M_PI / 180) * H_dash;

    double e0 = asin((sin(latitude)*sin(delta_dash)) + (cos(latitude)*cos(delta_dash)*cos(H_dash)));
    e0 = (e0 * 180) / M_PI;

    return e0;
}

double getAtmosphericRefractionCorrection(double P, double T, double e0)
{
    double temp = (M_PI / 180) * (e0 + (10.3 / (e0 + 5.11)));
    // double temp = e0 + (10.3 / (e0 + 5.11));
    temp = tan(temp);
    temp = (temp * 180) / M_PI;

    double delta_e = (P / 1010) * (283 / (273 + T)) * (1.02 / (60 * temp));

    return delta_e;
}

double getTopocentricElevationAngle(double e0, double delta_e)
{
    double e = e0 + delta_e;

    return e;
}

double getTopocentricZenithAngle(double e)
{
    double theta = 90 - e;

    return theta;
}

double getTopocentricAzimuthAngle(double H_dash, double latitude, double delta_dash)
{
    H_dash = (M_PI / 180) * H_dash;
    latitude = (M_PI / 180) * latitude;
    delta_dash = (M_PI / 180) * delta_dash;

    double temp = (cos(H_dash) * sin(latitude)) - (tan(delta_dash) * cos(latitude));

    double azimuth = atan2(sin(H_dash), temp);
    azimuth = (azimuth * 180) / M_PI;

    azimuth = azimuth / 360;
    double fraction = fabs(azimuth - floor(fabs(azimuth)));
    if (azimuth > 0)
    {
        azimuth = 360 * fraction;
    }
    else
    {
        azimuth = 360 - (360 * fraction);
    }

    azimuth = azimuth + 180;

    azimuth = azimuth / 360;
    fraction = fabs(azimuth - floor(fabs(azimuth)));
    if (azimuth > 0)
    {
        azimuth = 360 * fraction;
    }
    else
    {
        azimuth = 360 - (360 * fraction);
    }

    return azimuth;
}


float calculateAngleAdjustment(float flux1, float flux2)
{
    return ((flux1 / (flux1 + flux2)) * 180) - 90;
}

float decideAdjustment(float fluxSensor1, float fluxSensor2)
{
    // Calculate flux difference
    float fluxDifference = fabs(fluxSensor1 - fluxSensor2);


    // Check if the flux difference is greater than the threshold
    if (fluxDifference > 50)
    {
    	float angleAdjustment = 0;
        if (fluxSensor1 > fluxSensor2)
        {
            // Calculate angle adjustment
        	angleAdjustment = calculateAngleAdjustment(fluxSensor1, fluxSensor2);
        }
        else
        {
            // Calculate angle adjustment
        	angleAdjustment = calculateAngleAdjustment(fluxSensor2, fluxSensor1);
        }
        return angleAdjustment;
    }
    else
    {
        return 0;
    }
}

double *caller(int day, int year, int month, float hour, float min, float sec, float zone, float latitude, float longitude)
{
    double elevation = 1655;

    double pressure = 841.1604;
    double temperature = 15;

    double day_in_decimal = getDayinDecimals(day, hour, min, sec);
    double julianDay = getJulianDay(year, month, day_in_decimal, zone);

    // printf("Julian Day = %lf\n", julianDay);

    double julianEphemerisDay = getJulianEphemerisDay(julianDay);

    double julianCentury = getJulianCentury(julianDay);

    double julianEphemerisCentury = getJulianEphemerisCentury(julianEphemerisDay);

    double julianEphemerisMillenium = getJulianEphemerisMillenium(julianEphemerisCentury);

    /*
    printf("Julian Ephemeris Century = %lf\n", julianEphemerisCentury);
    printf("Julian Ephemeris Millenium = %lf\n", julianEphemerisMillenium);
    */

    struct L0 *dataL0 = initL0();
    struct L1 *dataL1 = initL1();
    struct L2 *dataL2 = initL2();
    struct L3 *dataL3 = initL3();
    struct L4 *dataL4 = initL4();
    struct L5 *dataL5 = initL5();

    double L0 = getL0(dataL0, julianEphemerisMillenium);
    double L1 = getL1(dataL1, julianEphemerisMillenium);
    double L2 = getL2(dataL2, julianEphemerisMillenium);
    double L3 = getL3(dataL3, julianEphemerisMillenium);
    double L4 = getL4(dataL4, julianEphemerisMillenium);
    double L5 = getL5(dataL5, julianEphemerisMillenium);

    double heliocentric_longitude = getHeliocentricLongitude(L0, L1, L2, L3, L4, L5, julianEphemerisMillenium);

    // printf("Heliocentric Longitude = %lf\n", heliocentric_longitude);

    struct B0 *dataB0 = initB0();
    struct B1 *dataB1 = initB1();

    double B0 = getB0(dataB0, julianEphemerisMillenium);
    double B1 = getB1(dataB1, julianEphemerisMillenium);

    double heliocentric_latitude = getHeliocentricLatitude(B0, B1, julianEphemerisMillenium);

    // printf("Heliocentric Latitude = %lf\n", heliocentric_latitude);

    struct R0 *dataR0 = initR0();
    struct R1 *dataR1 = initR1();
    struct R2 *dataR2 = initR2();
    struct R3 *dataR3 = initR3();
    struct R4 *dataR4 = initR4();

    double R0 = getR0(dataR0, julianEphemerisMillenium);
    double R1 = getR1(dataR1, julianEphemerisMillenium);
    double R2 = getR2(dataR2, julianEphemerisMillenium);
    double R3 = getR3(dataR3, julianEphemerisMillenium);
    double R4 = getR4(dataR4, julianEphemerisMillenium);

    double earth_radiusVector = getEarthRadiusVector(R0, R1, R2, R3, R4, julianEphemerisMillenium);

    // printf("Earth Radius Vector = %lf\n", earth_radiusVector);

    double geocentric_longitude = getGeocentricLongitude(heliocentric_longitude);
    double geocentric_latitude = -1 * heliocentric_latitude;

    /*
    printf("Geocentric Longitude = %lf\n", geocentric_longitude);
    printf("Geocentric Latitude = %lf\n", geocentric_latitude);
    */

    double x0 = getMeanElongation_MoonFromSun(julianEphemerisCentury);
    double x1 = getMeanAnomalySun(julianEphemerisCentury);
    double x2 = getMeanAnomalyMoon(julianEphemerisCentury);
    double x3 = getMoonLatitudeArgument(julianEphemerisCentury);
    double x4 = getMoonAscendingNode(julianEphemerisCentury);

    const int Y[63][5] = {
    {0,0,0,0,1},
    {-2,0,0,2,2},
    {0,0,0,2,2},
    {0,0,0,0,2},
    {0,1,0,0,0},
    {0,0,1,0,0},
    {-2,1,0,2,2},
    {0,0,0,2,1},
    {0,0,1,2,2},
    {-2,-1,0,2,2},
    {-2,0,1,0,0},
    {-2,0,0,2,1},
    {0,0,-1,2,2},
    {2,0,0,0,0},
    {0,0,1,0,1},
    {2,0,-1,2,2},
    {0,0,-1,0,1},
    {0,0,1,2,1},
    {-2,0,2,0,0},
    {0,0,-2,2,1},
    {2,0,0,2,2},
    {0,0,2,2,2},
    {0,0,2,0,0},
    {-2,0,1,2,2},
    {0,0,0,2,0},
    {-2,0,0,2,0},
    {0,0,-1,2,1},
    {0,2,0,0,0},
    {2,0,-1,0,1},
    {-2,2,0,2,2},
    {0,1,0,0,1},
    {-2,0,1,0,1},
    {0,-1,0,0,1},
    {0,0,2,-2,0},
    {2,0,-1,2,1},
    {2,0,1,2,2},
    {0,1,0,2,2},
    {-2,1,1,0,0},
    {0,-1,0,2,2},
    {2,0,0,2,1},
    {2,0,1,0,0},
    {-2,0,2,2,2},
    {-2,0,1,2,1},
    {2,0,-2,0,1},
    {2,0,0,0,1},
    {0,-1,1,0,0},
    {-2,-1,0,2,1},
    {-2,0,0,0,1},
    {0,0,2,2,1},
    {-2,0,2,0,1},
    {-2,1,0,2,1},
    {0,0,1,-2,0},
    {-1,0,1,0,0},
    {-2,1,0,0,0},
    {1,0,0,0,0},
    {0,0,1,2,0},
    {0,0,-2,2,2},
    {-1,-1,1,0,0},
    {0,1,1,0,0},
    {0,-1,1,2,2},
    {2,-1,-1,2,2},
    {0,0,3,2,2},
    {2,-1,0,2,2},
    };

    const double abcd[63][4]={
    {-171996,-174.2,92025,8.9},
    {-13187,-1.6,5736,-3.1},
    {-2274,-0.2,977,-0.5},
    {2062,0.2,-895,0.5},
    {1426,-3.4,54,-0.1},
    {712,0.1,-7,0},
    {-517,1.2,224,-0.6},
    {-386,-0.4,200,0},
    {-301,0,129,-0.1},
    {217,-0.5,-95,0.3},
    {-158,0,0,0},
    {129,0.1,-70,0},
    {123,0,-53,0},
    {63,0,0,0},
    {63,0.1,-33,0},
    {-59,0,26,0},
    {-58,-0.1,32,0},
    {-51,0,27,0},
    {48,0,0,0},
    {46,0,-24,0},
    {-38,0,16,0},
    {-31,0,13,0},
    {29,0,0,0},
    {29,0,-12,0},
    {26,0,0,0},
    {-22,0,0,0},
    {21,0,-10,0},
    {17,-0.1,0,0},
    {16,0,-8,0},
    {-16,0.1,7,0},
    {-15,0,9,0},
    {-13,0,7,0},
    {-12,0,6,0},
    {11,0,0,0},
    {-10,0,5,0},
    {-8,0,3,0},
    {7,0,-3,0},
    {-7,0,0,0},
    {-7,0,3,0},
    {-7,0,3,0},
    {6,0,0,0},
    {6,0,-3,0},
    {6,0,-3,0},
    {-6,0,3,0},
    {-6,0,3,0},
    {5,0,0,0},
    {-5,0,3,0},
    {-5,0,3,0},
    {-5,0,3,0},
    {4,0,0,0},
    {4,0,0,0},
    {4,0,0,0},
    {-4,0,0,0},
    {-4,0,0,0},
    {-4,0,0,0},
    {3,0,0,0},
    {-3,0,0,0},
    {-3,0,0,0},
    {-3,0,0,0},
    {-3,0,0,0},
    {-3,0,0,0},
    {-3,0,0,0},
    {-3,0,0,0},
    };

    double *nutation = malloc(2 * sizeof(double));
    nutation = getNutation(Y, abcd, julianEphemerisCentury, x0, x1, x2, x3, x4);
    double nutation_longitude = nutation[0];
    double nutation_obliquity = nutation[1];

    /*
    printf("Nutation Longitude = %lf\n", nutation_longitude);
    printf("Nutation Obliquity = %lf\n", nutation_obliquity);
    */

    double mean_obliquity = getMeanObliquity(julianEphemerisMillenium / 10);
    double true_obliquity = (mean_obliquity / 3600) + nutation_obliquity;

    // printf("True Obliquity = %lf\n", true_obliquity);

    double aberration_correction = 20.4898 / (3600 * earth_radiusVector);
    double apparent_sun_longitude = geocentric_longitude + nutation_longitude + aberration_correction;

    // printf("Apparent Sun Longitude = %lf\n", apparent_sun_longitude);

    double apparent_siderealTime = getApparentSiderealTime(julianDay, julianCentury, nutation_longitude, true_obliquity);
    double sun_right_ascension = getSunRightAscension(apparent_sun_longitude, true_obliquity, geocentric_latitude);
    double sun_declination = getSunDeclination(apparent_sun_longitude, true_obliquity, geocentric_latitude);

    /*
    printf("Geocentric Sun Right Ascension = %lf\n", sun_right_ascension);
    printf("Geocentric Sun Declination = %lf\n", sun_declination);
    */

    double observer_localHour_angle = getObserverLocalHourAngle(apparent_siderealTime, longitude, sun_right_ascension);

    // printf("Observer Local Hour Angle = %lf\n", observer_localHour_angle);

    double equatorial_horizontal_parallax = getEquatorialHorizontalParallax(earth_radiusVector);
    double u = helper_getU(latitude);
    double x = helper_getX(u, elevation, latitude);
    double y = helper_getY(u, elevation, latitude);
    double parallax_sunright_ascension = getParallax_SunRightAscension(x, equatorial_horizontal_parallax, observer_localHour_angle, sun_declination);
    double topocentric_sunright_ascension = getTopocentricSunRightAscension(sun_right_ascension, parallax_sunright_ascension);
    double topcentric_sun_declination = getTopocentricSunDeclination(sun_declination, y, x, equatorial_horizontal_parallax, parallax_sunright_ascension, observer_localHour_angle);
    double topocentric_local_hour_angle = getTopocentricLocalHourAngle(observer_localHour_angle, parallax_sunright_ascension);

    /*
    printf("Topocentric Sun Right Ascension = %lf\n", topocentric_sunright_ascension);
    printf("Topocentric Sun Declination = %lf\n", topcentric_sun_declination);
    printf("Topocentric Local Hour Angle = %lf\n", topocentric_local_hour_angle);
    */

    double e0 = getIncompleteElevationAngle(latitude, topcentric_sun_declination, topocentric_local_hour_angle);
    double delta_e = getAtmosphericRefractionCorrection(pressure, temperature, e0);
    double topocentric_elevation_angle = getTopocentricElevationAngle(e0, delta_e);
    double topocentric_zenith_angle = getTopocentricZenithAngle(topocentric_elevation_angle);

    // printf("Topocentric Zenith Angle = %lf\n", topocentric_zenith_angle);

    double topocentric_azimuth_angle = getTopocentricAzimuthAngle(topocentric_local_hour_angle, latitude, topcentric_sun_declination);

    // printf("Topocentric Azimuth Angle = %lf\n", topocentric_azimuth_angle);

    double *angles = malloc(2 * sizeof(double));
    angles[0] = topocentric_zenith_angle;
    angles[1] = topocentric_azimuth_angle;

    return angles;
}



//MAGNETOMETER
void  hardIronCal(float* xCal, float* yCal, float* zCal);
float getDirectionAngle(int16_t xMag, int16_t yMag, int16_t zMag, float xCal, float yCal, float zCal);
void setupMag();
float magnetometerData(float xCal,float yCal,float zCal);

//GPS
void getGpsData(int* lat, int* longi, int* time, int* date, char* longDir, char* latDir);

//VOLTAGE MEASUREMENT
float getSolarPanelVoltage();

//LIGHT SENSOR
void setupLightSensor(uint8_t lightAddress);
float getLightData(uint8_t lightAddress);

//MOTOR CONTROL
void microDelay (uint16_t delay);
void setupMotorRod();
void turnOffMotorRod();
void rotateMotorRod(float angle);
void setClockwiseRod();
void setCounterClockwiseRod();

void setupMotorSusan();
void turnOffMotorSusan();
void rotateMotorSusan(float angle);
void setClockwiseSusan();
void setCounterClockwiseSusan();

//BLUETOOTH
void bluetoothSend(float voltageMeasured, float direction, float lat, float longi);

#define ROD_DIR_PIN GPIO_PIN_11
#define ROD_DIR_PORT GPIOC
#define ROD_STEP_PIN GPIO_PIN_10
#define ROD_STEP_PORT GPIOC
#define ROD_SLEEP_PIN GPIO_PIN_2
#define ROD_SLEEP_PORT GPIOD
#define ROD_ENABLE_PIN GPIO_PIN_12
#define ROD_ENABLE_PORT GPIOC

#define SUSAN_DIR_PIN GPIO_PIN_6
#define SUSAN_DIR_PORT GPIOB
#define SUSAN_STEP_PIN GPIO_PIN_5
#define SUSAN_STEP_PORT GPIOB
#define SUSAN_SLEEP_PIN GPIO_PIN_7
#define SUSAN_SLEEP_PORT GPIOB
#define SUSAN_ENABLE_PIN GPIO_PIN_4
#define SUSAN_ENABLE_PORT GPIOB

//LEDs
#define DEBUG_LED_PIN GPIO_PIN_8
#define DEBUG_LED_PORT GPIOC
#define SMART_PANEL_LED_PIN GPIO_PIN_7
#define SMART_PANEL_LED_PORT GPIOC
///////////////////////////////////////////////////////////////////////////////////////////

//VARIABLES
//MAGNETOMETER
//Device Address
uint8_t magAddress = 0x1E;

//Register Addresses
uint8_t dataRegister = 0x68;
uint8_t cfgRegA = 0x60;
uint8_t cfgRegC = 0x62;

//Values to send
uint8_t initRegA = 0x0C;
uint8_t disRegA = 0x0E;
uint8_t initRegC = 0x10;
///////////////////////////////////
//GPS
//Address
uint16_t gpsAddress = 0x42;
///////////////////////////////////
//LIGHT SENSORS
uint8_t lightAddressGND = 0x44;
uint8_t lightAddressVDD = 0x45;
uint8_t lightAddressSDA = 0x46;
uint8_t lightAddressSCL = 0x47;
///////////////////////////////////
//Motor Control
//Motor Control Variables
int stepDelay = 500; // 1000us more delay means less speed
int stepsPerRev = 200;
float stepAngle = 1.8;
int gearRatio = 40;
///////////////////////////////////

int main(void)
{
	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();
	MX_ADC1_Init();
	MX_I2C1_Init();
	MX_I2C3_Init();
	MX_TIM1_Init();
	MX_USART1_UART_Init();


	///////////////////////////////////////////////////////////////
	//GPS
	int lat = -1;
	int longi = -1;
	int date = -1;
	char longiDir = 'x';
	char latDir = 'x';
	int time = -1;

	for (int i = 0; i < 100; i++) {
	 getGpsData(&lat, &longi, &time, &date, &longiDir, &latDir);
	}
	lat /= 100;
	longi /= 100;

	//////////////////////////////////////////////////////////////
	//LIGHT SENSOR
	setupLightSensor(lightAddressGND);
	setupLightSensor(lightAddressVDD);
	setupLightSensor(lightAddressSDA);
	setupLightSensor(lightAddressSCL);

	//////////////////////////////////////////////////////////////
	//SOLAR PANEL VOLTAGE
	float solarPanelVoltage = 0;

	//////////////////////////////////////////////////////////////
	//MOTOR CONTROL
	HAL_TIM_Base_Start(&htim1);
	///////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////
	//If no valid data -> gamer Mode On

	if (lat == '-1' || longi == '-1' || time == '-1' || date == '-1' ||
	    longiDir == 'x' || latDir == 'x') {
		lightFollowOnlyMode();
	}

	//////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////
	//MAGNETOMETER
	float xCal, yCal, zCal;
	xCal = 0;
	yCal = 0;
	zCal = 0;
	setupMag();
	hardIronCal(&xCal, &yCal, &zCal);

//	//Direction Angle
	float direction = magnetometerData(xCal, yCal, zCal);

	//Finding Azimuth and Zenith Angles for initialization
	float sec = time % 100;
	time /= 100;
	float min = time % 100;
	time /= 100;
    float hour = (time % 100) + 5;

    float year = (date % 100) + 2000;
    date /= 100;
    float month = date % 100;
    date /= 100;
    float day = date % 100;

    if (longiDir == 'W') {
    	longi *= -1;
    }
    if (latDir == 'S') {
    	lat *= -1;
    }

	double *angles = caller(day, year, month, hour, min, sec, 0, lat, longi);
	int zenithAngle = ((int)angles[0]) % 90;
	int azimuthAngle = ((int) angles[1]) % 180;

	solarPanelVoltage = getSolarPanelVoltage();
	bluetoothSend(solarPanelVoltage, direction, lat, longi);

	//Rotate to match azimuth angle

	int rotationSusanAngle = azimuthAngle - direction;
	setupMotorSusan();
	if (rotationSusanAngle < 0) {
		setCounterClockwiseSusan();
		rotationSusanAngle += 360;
	}
	else {
		setClockwiseSusan();
	}
	rotateMotorSusan(rotationSusanAngle * gearRatio);
	turnOffMotorSusan();

	//Have always the same initial state
	float currentPosition = 90;
	float rotationRodAngle  = currentPosition - zenithAngle;
	currentPosition = rotationRodAngle;
	setupMotorRod();
	setClockwiseRod();
	rotateMotorRod(rotationRodAngle * gearRatio); //multiply by t10 due to gear ratio
	turnOffMotorRod();

	int counter = 0;
	while (1)
	{
		//Adjust every 5 minutes
		if ((HAL_GetTick() - counter) > 300000) {
			for (int i = 0; i < 50; i++) {
			 getGpsData(&lat, &longi, &time, &date, &longiDir, &latDir);
			}
			lat /= 100;
			longi /= 100;

			if (longiDir == 'W') {
				longi *= -1;
			}
			if (latDir == 'S') {
				lat *= -1;
			}

			direction = magnetometerData(xCal, yCal, zCal);
			sec = time % 100;
			time /= 100;
			min = time % 100;
			time /= 100;
			hour = time % 100;

			year = (date % 100) + 2000;
			date /= 100;
			month = date % 100;
			date /= 100;
			day = date % 100;

			angles = caller(day, year, month, hour, min, sec, 0, lat, longi);
			zenithAngle = ((int) angles[0]) % 90;
			azimuthAngle = ((int) angles[1]) % 180;

			//Rotate to match azimuth angle
			setupMotorSusan();
			float rotationSusanAngle = azimuthAngle - direction;
			if (rotationSusanAngle < 0) {
				setCounterClockwiseSusan();
				rotationSusanAngle += 360;
			}
			else {
				setClockwiseSusan();
			}
			rotateMotorSusan(rotationSusanAngle * gearRatio);
			turnOffMotorSusan();

			//Have always the same initial state
			float desiredAngle = 90 - zenithAngle;
			rotationRodAngle  = currentPosition - desiredAngle;
			setupMotorRod();
			if (currentPosition < desiredAngle) {
				setClockwiseRod();
			}
			else {
				setCounterClockwiseRod();
			}
			rotateMotorRod(rotationRodAngle * gearRatio); //multiply by t10 due to gear ratio
			turnOffMotorRod();
			currentPosition = desiredAngle;

			counter = HAL_GetTick();

			solarPanelVoltage = getSolarPanelVoltage();
			bluetoothSend(solarPanelVoltage, direction, lat, longi);
		}
	}
}

void lightFollowOnlyMode() {
	//Later on, if everything's based on interrupts, we deactivate all interrupts
	int motorCounter = 0;
	while (1) {
		if ((HAL_GetTick() - motorCounter) > 500) {
			HAL_GPIO_WritePin(SMART_PANEL_LED_PORT, SMART_PANEL_LED_PIN, GPIO_PIN_SET);
			HAL_GPIO_WritePin(DEBUG_LED_PORT, DEBUG_LED_PIN, GPIO_PIN_RESET);

			float lightDataVDD = getLightData(lightAddressVDD);
			float lightDataGND = getLightData(lightAddressGND);
			float lightDataSDA = getLightData(lightAddressSDA);
			float lightDataSCL = getLightData(lightAddressSCL);

			setupMotorRod();

			if ((lightDataSDA - lightDataSCL)  > 60) {
				setCounterClockwiseRod(); //up
				rotateMotorRod(361);
			}
			else if ((lightDataSCL - lightDataSDA) > 60) {
				setClockwiseRod(); //down
				rotateMotorRod(361);
			}

			turnOffMotorRod();


			//Figuring our rotational movement
			setupMotorSusan();

			if ((lightDataVDD - lightDataGND)  > 60) {
				setCounterClockwiseSusan();
				rotateMotorSusan(361);
			}
			else if ((lightDataGND - lightDataVDD) > 60) {
				setClockwiseSusan();
				rotateMotorSusan(361);
			}

			turnOffMotorSusan();



//			Voltage measurement and sending
			float solarPanelVoltage = getSolarPanelVoltage();
			bluetoothSend(solarPanelVoltage, -1, -1, -1);


			motorCounter = HAL_GetTick();
		}

		HAL_GPIO_WritePin(SMART_PANEL_LED_PORT, SMART_PANEL_LED_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(DEBUG_LED_PORT, DEBUG_LED_PIN, GPIO_PIN_SET);

	}
}

void bluetoothSend(float voltageMeasured, float direction, float lat, float longi) {
	if (direction == -1 || lat == -1 || longi == -1) {
		char msg[26];

		sprintf((char*)msg,"Voltage measured: %.4f\r\n", voltageMeasured);

		HAL_UART_Transmit_IT(&huart1,msg, sizeof(msg));
	}
	else {
		char msg[26];

		sprintf((char*)msg,"Voltage measured: %.2f\r\n", voltageMeasured);
		HAL_UART_Transmit_IT(&huart1,msg, sizeof(msg));
		microDelay(stepDelay);

		sprintf((char*)msg,"Direction: %.2f\r\n", direction);
		HAL_UART_Transmit_IT(&huart1,msg, sizeof(msg));
		microDelay(stepDelay);

		sprintf((char*)msg,"Latitude: %.2f\r\n", lat);
		HAL_UART_Transmit_IT(&huart1,msg, sizeof(msg));
		microDelay(stepDelay);

		sprintf((char*)msg,"Longitude: %.2f\r\n", longi);
		HAL_UART_Transmit_IT(&huart1,msg, sizeof(msg));
		microDelay(stepDelay);

	}
}

///////////////////////////////////////////////////////////////
//MOTOR CONTROL
void rotateMotorRod(float angle) {

	int steps = (angle / stepAngle);

	for(int x=0; x < steps; x++)
	{
		HAL_GPIO_WritePin(ROD_STEP_PORT, ROD_STEP_PIN, GPIO_PIN_SET);
		microDelay(stepDelay);
		HAL_GPIO_WritePin(ROD_STEP_PORT, ROD_STEP_PIN, GPIO_PIN_RESET);
		microDelay(stepDelay);
	}
}

void rotateMotorSusan(float angle) {

	int steps = (angle / stepAngle);

	for(int x=0; x < steps; x++)
	{
		HAL_GPIO_WritePin(SUSAN_STEP_PORT, SUSAN_STEP_PIN, GPIO_PIN_SET);
		microDelay(stepDelay);
		HAL_GPIO_WritePin(SUSAN_STEP_PORT, SUSAN_STEP_PIN, GPIO_PIN_RESET);
		microDelay(stepDelay);
	}
}

void setClockwiseRod() {
	HAL_GPIO_WritePin(ROD_DIR_PORT, ROD_DIR_PIN, GPIO_PIN_SET);
}

void setClockwiseSusan() {
	HAL_GPIO_WritePin(SUSAN_DIR_PORT, SUSAN_DIR_PIN, GPIO_PIN_SET);
}

void setCounterClockwiseRod() {
	HAL_GPIO_WritePin(ROD_DIR_PORT, ROD_DIR_PIN, GPIO_PIN_RESET);
}

void setCounterClockwiseSusan() {
	HAL_GPIO_WritePin(SUSAN_DIR_PORT, SUSAN_DIR_PIN, GPIO_PIN_RESET);
}

void microDelay (uint16_t delay)
{
  __HAL_TIM_SET_COUNTER(&htim1, 0);
  while (__HAL_TIM_GET_COUNTER(&htim1) < delay);
}

void setupMotorRod() {
	HAL_GPIO_WritePin(ROD_SLEEP_PORT, ROD_SLEEP_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ROD_ENABLE_PORT, ROD_ENABLE_PIN, GPIO_PIN_RESET);
}

void setupMotorSusan() {
	HAL_GPIO_WritePin(SUSAN_SLEEP_PORT, SUSAN_SLEEP_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SUSAN_ENABLE_PORT, SUSAN_ENABLE_PIN, GPIO_PIN_RESET);
}

void turnOffMotorRod() {
	HAL_GPIO_WritePin(ROD_SLEEP_PORT, ROD_SLEEP_PIN, GPIO_PIN_RESET);
}

void turnOffMotorSusan() {
	HAL_GPIO_WritePin(SUSAN_SLEEP_PORT, SUSAN_SLEEP_PIN, GPIO_PIN_RESET);
}
///////////////////////////////////////////////////////////////////

void setupLightSensor(uint8_t lightAddress) {
	uint8_t data_send[3] = {0};
	data_send[0] = 0x0A;
	data_send[1] = 0x32;
	data_send[2] = 0xF8;
	while (HAL_I2C_Master_Transmit(&hi2c3, (lightAddress << 1), data_send, 3, 20) != HAL_OK){}
}

float getLightData(uint8_t lightAddress) {
	uint8_t data_send[4] = {0};
	uint8_t data_in [2] = {0};
	uint16_t exp;
	uint16_t man;
	uint16_t alldata;
	float ans = 0;

	data_send[0] = 0x00;
	while (HAL_I2C_GetState(&hi2c3) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Transmit(&hi2c3, (lightAddress << 1), data_send, 1, 20) != HAL_OK){}
	while (HAL_I2C_GetState(&hi2c3) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Receive(&hi2c3, (lightAddress << 1 | 1), data_in, 2, HAL_MAX_DELAY)){}

	alldata = data_in[0];
	alldata = (alldata << 8) | data_in[1];
	man = alldata & 0x0FFF;
	exp = (alldata & 0xF000) >> 12;

	ans = man * (0.01 * pow(2,exp));

	return ans;
}


float getSolarPanelVoltage() {
	unsigned int analogValue = 0;

	HAL_ADC_Start(&hadc1);
	if(HAL_ADC_PollForConversion(&hadc1, 300) == HAL_OK) {
		analogValue = HAL_ADC_GetValue(&hadc1);
	}

	HAL_ADC_Stop(&hadc1);

	//Value obtained from voltage divider
	int voltageDividerLargeRes = 13;
	float voltageDividerSmallRes = 3;
	float measuredVoltage = (analogValue * 3.3 /  4096);
	float solarPanelVoltage = (measuredVoltage * (voltageDividerLargeRes + voltageDividerSmallRes) / voltageDividerSmallRes);

	return solarPanelVoltage;
}

void getGpsData(int* lat, int* longi, int* time, int* date, char* longDir, char* latDir) {
	uint8_t gpsDataTx [10] = {0}; //Data to send to GPS module
	uint8_t gpsDataReady [2] = {0}; //Data to check if gps module ready
	uint8_t gpsDataRx1 [32] = {0}; //Buffer 1
	uint8_t gpsDataRx2 [32] = {0}; //Buffer 2
	uint8_t gpsDataRx3 [32] = {0}; //Buffer 3
	uint8_t gpsDataRx4 [4] = {0}; //Buffer 4

	//////////////////////////////////////////////////////////////////////////////////////////
	//INITIALIZATION
	gpsDataTx[0] = 0xB5;
	gpsDataTx[1] = 0x62;
	gpsDataTx[2] = 0xF0;
	gpsDataTx[3] = 0x04;

	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Transmit(&hi2c1, (gpsAddress << 1), gpsDataTx, 4, 20) != HAL_OK){}

//		/////////////////////////////////////////////////////////////////////////////////////////
//
//		//WAITING FOR DATA READY
	gpsDataReady[0] = 0;
	gpsDataReady[1] = 0;
	while ((gpsDataReady[0] == 0) && (gpsDataReady[1] == 0)) //We leave as soon as read something non zero
	{
		gpsDataTx[0] = 0xFD;
		HAL_I2C_Master_Transmit(&hi2c1, (gpsAddress << 1), gpsDataTx, 1, HAL_MAX_DELAY);
		gpsDataReady[0] = 0;
		gpsDataReady[1] = 0;
		HAL_I2C_Master_Receive(&hi2c1, (gpsAddress << 1 | 1), gpsDataReady, 2, HAL_MAX_DELAY);
	}


//		//Buffer 1
	gpsDataTx[0] = 0xFF;
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Transmit(&hi2c1, (gpsAddress << 1), gpsDataTx, 1, 20) != HAL_OK){}
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Receive(&hi2c1, (gpsAddress << 1 | 1), gpsDataRx1, 32, HAL_MAX_DELAY)){}
//
//		//Buffer 2
	gpsDataTx[0] = 0xFF;
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Transmit(&hi2c1, (gpsAddress << 1), gpsDataTx, 1, 20) != HAL_OK){}
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Receive(&hi2c1, (gpsAddress << 1 | 1), gpsDataRx2, 32, HAL_MAX_DELAY)){}
//
//		//Buffer 3
	gpsDataTx[0] = 0xFF;
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Transmit(&hi2c1, (gpsAddress << 1), gpsDataTx, 1, 20) != HAL_OK){}
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Receive(&hi2c1, (gpsAddress << 1 | 1), gpsDataRx3, 32, HAL_MAX_DELAY)){}

	//Buffer 4
	gpsDataTx[0] = 0xFF;
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Transmit(&hi2c1, (gpsAddress << 1), gpsDataTx, 1, 20) != HAL_OK){}
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
	while (HAL_I2C_Master_Receive(&hi2c1, (gpsAddress << 1 | 1), gpsDataRx4, 4, HAL_MAX_DELAY)){}


	//Create concaterated buffer
	 size_t totalSize = sizeof(gpsDataRx1) + sizeof(gpsDataRx2) + sizeof(gpsDataRx3) + sizeof(gpsDataRx4);

	// Allocate memory for the concatenated array
	uint8_t *concatenatedArray = (uint8_t *)malloc(totalSize);

	// Copy the contents of each array into the concatenated array
	size_t offset = 0;
	memcpy(concatenatedArray + offset, gpsDataRx1, sizeof(gpsDataRx1));
	offset += sizeof(gpsDataRx1);
	memcpy(concatenatedArray + offset, gpsDataRx2, sizeof(gpsDataRx2));
	offset += sizeof(gpsDataRx2);
	memcpy(concatenatedArray + offset, gpsDataRx3, sizeof(gpsDataRx3));
	offset += sizeof(gpsDataRx3);
	memcpy(concatenatedArray + offset, gpsDataRx4, sizeof(gpsDataRx4));


	//String comparisons
	const char* GNRMC = "NRMC,";
	const char* minus1 = "-1";
	const char* comma = ",";

	char* gpsDataList[12];

	for (int i = 0; i < 12; i++) {
	    gpsDataList[i] = "-1";
	}

	//Parsing the data and updating our values

	if (strncmp(concatenatedArray, GNRMC, strlen(GNRMC)) == 0) {
		char* individualGpsData = strtok(concatenatedArray, ","); // Initialize strtok with the buffer

		int index = 0;

		while (individualGpsData != NULL) {
			gpsDataList[index] = individualGpsData; // Store the token in an array
			individualGpsData = strtok(NULL, ","); // Get the next token
			index++;
		}
	}

	//if the value is different from both -1 and ,, we update our time
	if ((strncmp(gpsDataList[1], minus1, strlen(minus1)) != 0) &&
	   (strncmp(gpsDataList[1], comma, strlen(comma)) != 0)) {
		*time = atoi(gpsDataList[1]);
	}

	if ((strncmp(gpsDataList[3], minus1, strlen(minus1)) != 0) &&
	   (strncmp(gpsDataList[3], comma, strlen(comma)) != 0)) {
		*lat = atoi(gpsDataList[3]);
	}

	if ((strncmp(gpsDataList[4], minus1, strlen(minus1)) != 0) &&
	   (strncmp(gpsDataList[4], comma, strlen(comma)) != 0)) {
		strcpy(latDir, gpsDataList[4]);
	}

	if ((strncmp(gpsDataList[5], minus1, strlen(minus1)) != 0) &&
	   (strncmp(gpsDataList[5], comma, strlen(comma)) != 0)) {
		*longi = atoi(gpsDataList[5]);
	}

	if ((strncmp(gpsDataList[6], minus1, strlen(minus1)) != 0) &&
	   (strncmp(gpsDataList[6], comma, strlen(comma)) != 0)) {
		strcpy(longDir, gpsDataList[6]);
	}

	if ((strncmp(gpsDataList[8], minus1, strlen(minus1)) != 0) &&
	   (strncmp(gpsDataList[8], comma, strlen(comma)) != 0)) {
		*date = atoi(gpsDataList[8]);
	}

	free(concatenatedArray);
}

float magnetometerData(float xCal,float yCal,float zCal) {
	int16_t magnetometerVal [3] = {0};

	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &dataRegister, 1, 20) != HAL_OK){} //send data register address
	while (HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), magnetometerVal, 6, HAL_MAX_DELAY)){}

	int16_t xMag = magnetometerVal[0];

	int16_t yMag = magnetometerVal[1];

	int16_t zMag = magnetometerVal[2];

	float direction = getDirectionAngle(xMag, yMag, zMag, xCal, yCal, zCal);

	return direction;
}


void setupMag() {
//	uint8_t magAddress = 0x1E;
//
//	//Register Addresses
//	uint8_t dataRegister = 0x68;
//	uint8_t cfgRegA = 0x60;
//	uint8_t cfgRegC = 0x62;
//
//	//Values to send
//	uint8_t initRegA = 0x0C;
//	uint8_t disRegA = 0x0E;
//	uint8_t initRegC = 0x10;
	uint8_t magSetupRX = 0;

	uint8_t magSetupTX [2] = {0};

	//1
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &cfgRegC, 1, HAL_MAX_DELAY) != HAL_OK){} //send address pointer

	//2
	HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), &magSetupRX, 1, HAL_MAX_DELAY);


	//3
	magSetupTX[0] = cfgRegC;
	magSetupTX[1] = initRegC;
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), magSetupTX, 2, HAL_MAX_DELAY) != HAL_OK){}

	//4
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &cfgRegA, 1, HAL_MAX_DELAY) != HAL_OK){}

	//5
	HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), &magSetupRX, 1, HAL_MAX_DELAY);

	//6
	magSetupTX[0] = cfgRegA;
	magSetupTX[1] = disRegA;
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), magSetupTX, 2, HAL_MAX_DELAY) != HAL_OK){}

	//7
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &cfgRegA, 1, HAL_MAX_DELAY) != HAL_OK){}

	//8
	HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), &magSetupRX, 1, HAL_MAX_DELAY);

	//9
	magSetupTX[0] = cfgRegA;
	magSetupTX[1] = disRegA;
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), magSetupTX, 2, HAL_MAX_DELAY) != HAL_OK){}

	//10
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &cfgRegC, 1, HAL_MAX_DELAY) != HAL_OK){}

	//11
	HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), &magSetupRX, 1, HAL_MAX_DELAY);


	//12
	magSetupTX[0] = cfgRegC;
	magSetupTX[1] = initRegC;
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), magSetupTX, 2, HAL_MAX_DELAY) != HAL_OK){}

	//13
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &cfgRegA, 2, HAL_MAX_DELAY) != HAL_OK){}

	//14
	HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), &magSetupRX, 1, HAL_MAX_DELAY);


	//15
	magSetupTX[0] = cfgRegA;
	magSetupTX[1] = initRegA;
	while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), magSetupTX, 2, HAL_MAX_DELAY) != HAL_OK){}
}

void hardIronCal(float* xCal, float* yCal, float* zCal) {
  int16_t magnetometerVal [3] = {0};

  while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &dataRegister, 1, 20) != HAL_OK){} //send data register address
  while (HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), magnetometerVal, 6, HAL_MAX_DELAY)){}

  int16_t xMax = magnetometerVal[0];
  int16_t xMin = magnetometerVal[0];

  int16_t yMax = magnetometerVal[1];
  int16_t yMin = magnetometerVal[1];

  int16_t zMax = magnetometerVal[2];
  int16_t zMin = magnetometerVal[2];

  int i = 0;
  int motorCounter = 0;
  while (i != 54) {

		while (HAL_I2C_Master_Transmit(&hi2c1, (magAddress << 1), &dataRegister, 1, 20) != HAL_OK){} //send data register address
		while (HAL_I2C_Master_Receive(&hi2c1, (magAddress << 1 | 1), magnetometerVal, 6, HAL_MAX_DELAY)){}

		//Updating xMax and xMin
		if (magnetometerVal[0] > xMax) {
		  xMax = magnetometerVal[0];
		}

		else if (magnetometerVal[0] < xMin) {
		  xMin = magnetometerVal[0];
		}

		//Updating yMax and yMin
		if (magnetometerVal[1] > yMax) {
		  yMax = magnetometerVal[1];
		}

		else if (magnetometerVal[1] < yMin) {
		  yMin = magnetometerVal[1];
		}

		//Updating zMax and zMin
		if (magnetometerVal[2] > zMax) {
		  zMax = magnetometerVal[2];
		}

		else if (magnetometerVal[2] < zMin) {
		  zMin = magnetometerVal[2];
		}

		if ((HAL_GetTick() - motorCounter) > 1000) {
			setupMotorSusan();
			if (i < 16) {
				setClockwiseSusan();
				rotateMotorSusan(361);
			}
			else if ( i >= 16 && i < 43){
				setCounterClockwiseSusan();
				rotateMotorSusan(361);
			}
			else {
				setClockwiseSusan();
				rotateMotorSusan(361);
			}
			turnOffMotorSusan();

			i++;
			motorCounter = HAL_GetTick();
		}
	}

  //finding the calibration values

  *xCal = (xMax + xMin) / 2;
  *yCal = (yMax + yMin) / 2;
  *zCal = (zMax + zMin) / 2;
}

float getDirectionAngle(int16_t xMag, int16_t yMag, int16_t zMag, float xCal, float yCal, float zCal){
  xMag -= xCal;
  yMag -= yCal;
  zMag -= zCal;

  float D = atan2(xMag, yMag) * (180 / M_PI);
  D -= 55;
  if (D > 360) {
    D -= 360;
  }
  else if (D < 0){
    D += 360;
  }

  return D;
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, Extra_PC0_Pin|Extra_PC1_Pin|Extra_PC4_Pin|Solar_Panel_Voltage_LED_Pin
                          |Debug_LED_Pin|Step_Rod_Logic_Pin|Direction_Rod_Logic_Pin|Enable_Rod_Logic_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Extra_PA1_Pin|Extra_PA2_Pin|Extra_PA3_Pin|Extra_PA4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Extra_PB12_Pin|Extra_PB13_Pin|Extra_PB14_Pin|Extra_PB15_Pin
                          |Extra_PB3_Pin|Enable_Susan_Logic_Pin|Step_Susan_Logic_Pin|Direction_Susan_Logic_Pin
                          |nSleep_Susan_Logic_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(nSleep_Rod_Logic_GPIO_Port, nSleep_Rod_Logic_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : Extra_PC0_Pin Extra_PC1_Pin Extra_PC4_Pin Solar_Panel_Voltage_LED_Pin
                           Debug_LED_Pin Step_Rod_Logic_Pin Direction_Rod_Logic_Pin Enable_Rod_Logic_Pin */
  GPIO_InitStruct.Pin = Extra_PC0_Pin|Extra_PC1_Pin|Extra_PC4_Pin|Solar_Panel_Voltage_LED_Pin
                          |Debug_LED_Pin|Step_Rod_Logic_Pin|Direction_Rod_Logic_Pin|Enable_Rod_Logic_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Extra_PA1_Pin Extra_PA2_Pin Extra_PA3_Pin Extra_PA4_Pin */
  GPIO_InitStruct.Pin = Extra_PA1_Pin|Extra_PA2_Pin|Extra_PA3_Pin|Extra_PA4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : Extra_PB12_Pin Extra_PB13_Pin Extra_PB14_Pin Extra_PB15_Pin
                           Extra_PB3_Pin Enable_Susan_Logic_Pin Step_Susan_Logic_Pin Direction_Susan_Logic_Pin
                           nSleep_Susan_Logic_Pin */
  GPIO_InitStruct.Pin = Extra_PB12_Pin|Extra_PB13_Pin|Extra_PB14_Pin|Extra_PB15_Pin
                          |Extra_PB3_Pin|Enable_Susan_Logic_Pin|Step_Susan_Logic_Pin|Direction_Susan_Logic_Pin
                          |nSleep_Susan_Logic_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : nSleep_Rod_Logic_Pin */
  GPIO_InitStruct.Pin = nSleep_Rod_Logic_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(nSleep_Rod_Logic_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
