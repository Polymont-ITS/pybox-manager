#include "toolkit/ovtk_all.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Toolkit;


// ********************************************************************************************************************
//                                                                                                                    *
// VIM Replace string to easily add enumeration values :                                                              *
//                                                                                                                    *
// :%s/#define \([A-Za-z0-9_]\+\).*/	typeManager.registerEnumerationEntry(OV_TypeId_Stimulation, "\1", \1);/g   *
//                                                                                                                    *
// ********************************************************************************************************************



bool Toolkit::initialize(const IKernelContext& ctx)
{

	ITypeManager& typeManager = ctx.getTypeManager();

	initializeStimulationList(ctx);

	// Register measurement units. See ovtk_defines.h for details.
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "?", OVTK_UNIT_Unspecified);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "-", OVTK_UNIT_Dimensionless);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "%", OVTK_UNIT_10_2_Percent);
//	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "percent", OVTK_UNIT_10_2_Percent);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "ppht", OVTK_UNIT_10_3_Parts_Per_Thousand);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "ppm", OVTK_UNIT_10_6_Parts_Per_Million);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "ppmd", OVTK_UNIT_10_9_Parts_Per_Milliard);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "ppb", OVTK_UNIT_10_12_Parts_Per_Billion);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "ppt", OVTK_UNIT_10_18_Parts_Per_Trillion);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "degree", OVTK_UNIT_Angle_Degree);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "rad", OVTK_UNIT_Angle_Radian);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g g-1", OVTK_UNIT_Grams_Per_Gram);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g kg-1", OVTK_UNIT_Grams_Per_Kilogram);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol mol-1", OVTK_UNIT_Moles_Per_Mole);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l l-1", OVTK_UNIT_Litres_Per_Litre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m m-3", OVTK_UNIT_Cubic_Metres_Per_Cubic_Metre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m cm-3", OVTK_UNIT_Cubic_Metres_Per_Cubic_Centimetre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "vol %", OVTK_UNIT_Volume_Percent);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "pH", OVTK_UNIT_Ph);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "drop", OVTK_UNIT_Drop);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "rbc", OVTK_UNIT_Red_Blood_Cells);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "beat", OVTK_UNIT_Beat);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "breath", OVTK_UNIT_Breath);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "cell", OVTK_UNIT_Cell);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "cough", OVTK_UNIT_Cough);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "sigh", OVTK_UNIT_Sigh);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "%PCV", OVTK_UNIT_Percent_Of_Packed_Cell_Volume);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m", OVTK_UNIT_Metres);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "yd", OVTK_UNIT_Yard);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "ft", OVTK_UNIT_Foot);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "in", OVTK_UNIT_Inch);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "lm-2", OVTK_UNIT_Litres_Per_Square_Metre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m-1", OVTK_UNIT_Per_Metre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m2", OVTK_UNIT_Square_Metres);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "in2", OVTK_UNIT_Square_Inch);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m-2", OVTK_UNIT_Per_Square_Metre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m3", OVTK_UNIT_Cubic_Metres);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l", OVTK_UNIT_Litres);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l_breath-1", OVTK_UNIT_Litres_Per_Breath);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l_beat-1", OVTK_UNIT_Litres_Per_Beat);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m-3", OVTK_UNIT_Per_Cubic_Metre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l-1", OVTK_UNIT_Per_Litre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g", OVTK_UNIT_Gram);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "lb", OVTK_UNIT_Pound);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "oz", OVTK_UNIT_Ounce);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g-1", OVTK_UNIT_Per_Gram);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "gm", OVTK_UNIT_Gram_Metre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g m m-2", OVTK_UNIT_Grams_Meter_Per_Square_Metre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "kg m2", OVTK_UNIT_Gram_Metre_Squared);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "kg m-2", OVTK_UNIT_Kilograms_Per_Square_Metre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g m-3", OVTK_UNIT_Grams_Per_Cubic_Metre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g cm-3", OVTK_UNIT_Grams_Per_Cubic_Centimetre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g l-1", OVTK_UNIT_Grams_Per_Litre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g cl-3", OVTK_UNIT_Grams_Per_Centilitre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g dl-3", OVTK_UNIT_Grams_Per_Decilitre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g ml-3", OVTK_UNIT_Grams_Per_Millilitre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "s", OVTK_UNIT_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "min", OVTK_UNIT_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "h", OVTK_UNIT_Hour);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "d", OVTK_UNIT_Day);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "weeks", OVTK_UNIT_Weeks);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mth", OVTK_UNIT_Months);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "y", OVTK_UNIT_Year);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "TOD", OVTK_UNIT_Time_Of_Day_Hh_Mm_Ss);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "DATE", OVTK_UNIT_Date_Yyyy_Mm_Dd);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "s-1", OVTK_UNIT_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "Hz", OVTK_UNIT_Hertz);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "min-1", OVTK_UNIT_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "h-1", OVTK_UNIT_Per_Hour);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "d-1", OVTK_UNIT_Per_Day);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "week-1", OVTK_UNIT_Per_Week);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mth-1", OVTK_UNIT_Per_Month);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "y-1", OVTK_UNIT_Per_Year);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "bpm", OVTK_UNIT_Beat_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "puls min-1", OVTK_UNIT_Puls_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "resp min-1", OVTK_UNIT_Respirations_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m s-1", OVTK_UNIT_Metres_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l min-1 m-2", OVTK_UNIT_Litres_Per_Minute_Per_Square_Meter);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m2 s-1", OVTK_UNIT_Square_Metres_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m3 s-1", OVTK_UNIT_Cubic_Metres_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m3 min-1", OVTK_UNIT_Cubic_Metres_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m3 h-1", OVTK_UNIT_Cubic_Metres_Per_Hour);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m3 d-1", OVTK_UNIT_Cubic_Metres_Per_Day);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l s-1", OVTK_UNIT_Litres_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l min-1", OVTK_UNIT_Litres_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l h-1", OVTK_UNIT_Litres_Per_Hour);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l d-1", OVTK_UNIT_Litres_Per_Day);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l kg-1", OVTK_UNIT_Litres_Per_Kilogram);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m3 kg-1", OVTK_UNIT_Cubic_Metres_Per_Kilogram);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m Pa-1s-1", OVTK_UNIT_Meter_Per_Pascal_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l min-1 mmHG-1", OVTK_UNIT_Litre_Per_Min_Per_Millimetre_Of_Mercury);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g s-1", OVTK_UNIT_Grams_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g m-1", OVTK_UNIT_Grams_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g h-1", OVTK_UNIT_Grams_Per_Hour);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g d-1", OVTK_UNIT_Grams_Per_Day);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g kg-1 s-1", OVTK_UNIT_Grams_Per_Kilogram_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g kg-1 m-1", OVTK_UNIT_Grams_Per_Kilogram_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g kg-1 h-1", OVTK_UNIT_Grams_Per_Kilogram_Per_Hour);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g kg-1 d-1", OVTK_UNIT_Grams_Per_Kilogram_Per_Day);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g l-1_s-1", OVTK_UNIT_Grams_Per_Litre_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g l-1 m-1", OVTK_UNIT_Grams_Per_Litre_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g l-1 h-1", OVTK_UNIT_Grams_Per_Litre_Per_Hour);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g l-1 d-1", OVTK_UNIT_Grams_Per_Litre_Per_Day);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g m-1 s-1", OVTK_UNIT_Grams_Per_Meter_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "gm s-1", OVTK_UNIT_Gram_Metres_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "Ns", OVTK_UNIT_Newton_Seconds);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "N", OVTK_UNIT_Newton);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "dyn", OVTK_UNIT_Dyne);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "Pa", OVTK_UNIT_Pascal);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mmHg", OVTK_UNIT_Millimetres_Of_Mercury);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "cm H2O", OVTK_UNIT_Centimetre_Of_Water);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "bar", OVTK_UNIT_Bar);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "J", OVTK_UNIT_Joules);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "eV", OVTK_UNIT_Electronvolts);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "W", OVTK_UNIT_Watts);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "Pa_s_m-3", OVTK_UNIT_Pascal_Second_Per_Cubic_Meter);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "Pa_s_l-1", OVTK_UNIT_Pascal_Second_Per_Litre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "dyne s cm-5", OVTK_UNIT_Dyne_Second_Per_Cm5);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l cmH2O-1", OVTK_UNIT_Litre_Per_Centimetre_Of_Water);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l mmHg-1", OVTK_UNIT_Litre_Per_Millimetre_Of_Mercury);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l Pa-1", OVTK_UNIT_Litre_Per_Pascal);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "cmH2O l-1", OVTK_UNIT_Centimetre_Of_Water_Per_Litre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mmHg l-1", OVTK_UNIT_Millimetre_Of_Mercury_Per_Litre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "Pa l-1", OVTK_UNIT_Pascal_Per_Litre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "A", OVTK_UNIT_Amperes);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "C", OVTK_UNIT_Coulombs);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "Ah", OVTK_UNIT_Amperes_Hour);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "A m-1", OVTK_UNIT_Amperes_Per_Metre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "V", OVTK_UNIT_Volts);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "Ohm", OVTK_UNIT_Ohms);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "Wm", OVTK_UNIT_Ohm_Metres);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "F", OVTK_UNIT_Farads);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "K", OVTK_UNIT_Kelvin);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "degC", OVTK_UNIT_Degree_Celcius);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "degF", OVTK_UNIT_Degree_Fahrenheit);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "K W-1", OVTK_UNIT_Kelvins_Per_Watt);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "cd", OVTK_UNIT_Candelas);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "osmole", OVTK_UNIT_Osmoles);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol", OVTK_UNIT_Moles);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "eq", OVTK_UNIT_Equivalent);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "osmol l-1", OVTK_UNIT_Osmoles_Per_Litre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol cm-3", OVTK_UNIT_Moles_Per_Cubic_Centimetre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol m-3", OVTK_UNIT_Moles_Per_Cubic_Metre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol l-1", OVTK_UNIT_Moles_Per_Litre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol ml-1", OVTK_UNIT_Moles_Per_Millilitre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "eq cm-3", OVTK_UNIT_Equivalents_Per_Cubic_Centimetre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "eq m-3", OVTK_UNIT_Equivalents_Per_Cubic_Metre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "eq l-1", OVTK_UNIT_Equivalents_Per_Litre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "eq ml-1", OVTK_UNIT_Equivalents_Per_Millilitre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "osmol kg-1", OVTK_UNIT_Osmoles_Per_Kilogram);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol kg-1", OVTK_UNIT_Moles_Per_Kilogram);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol s-1", OVTK_UNIT_Moles_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol min-1", OVTK_UNIT_Moles_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol h-1", OVTK_UNIT_Moles_Per_Hour);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol d-1", OVTK_UNIT_Moles_Per_Day);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "eq s-1", OVTK_UNIT_Equivalents_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "eq min-1", OVTK_UNIT_Equivalents_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "eq h-1", OVTK_UNIT_Equivalents_Per_Hour);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "eq d-1", OVTK_UNIT_Equivalents_Per_Day);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol kg-1 s-1", OVTK_UNIT_Moles_Per_Kilogram_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol kg-1 min-1", OVTK_UNIT_Moles_Per_Kilogram_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol kg-1 h-1", OVTK_UNIT_Moles_Per_Kilogram_Per_Hour);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol kg-1 d-1", OVTK_UNIT_Moles_Per_Kilogram_Per_Day);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "eq kg-1 s-1", OVTK_UNIT_Equivalents_Per_Kilogram_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "eq kg-1 min-1", OVTK_UNIT_Equivalents_Per_Kilogram_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "eq kg-1 h-1", OVTK_UNIT_Equivalents_Per_Kilogram_Per_Hour);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "eq kg-1 d-1", OVTK_UNIT_Equivalents_Per_Kilogram_Per_Day);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "i.u.", OVTK_UNIT_International_Unit);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "i.u. cm-3", OVTK_UNIT_International_Units_Per_Cubic_Centimetre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "i.u. m-3", OVTK_UNIT_International_Units_Per_Cubic_Meter);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "i.u. l-1", OVTK_UNIT_International_Units_Per_Litre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "i.u. ml-1", OVTK_UNIT_International_Units_Per_Millilitre);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "i.u. s-1", OVTK_UNIT_International_Units_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "i.u. min-1", OVTK_UNIT_International_Units_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "i.u. h-1", OVTK_UNIT_International_Units_Per_Hour);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "i.u. d-1", OVTK_UNIT_International_Units_Per_Day);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "i.u. kg-1_s-1", OVTK_UNIT_International_Units_Per_Kilogram_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "i.u. kg-1_min-1", OVTK_UNIT_International_Units_Per_Kilogram_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "i.u. kg-1_h-1", OVTK_UNIT_International_Units_Per_Kilogram_Per_Hour);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "i.u. kg-1_d-1", OVTK_UNIT_International_Units_Per_Kilogram_Per_Day);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "cmH2O l-1s-1", OVTK_UNIT_Centimetre_Of_Water_Per_Litre_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l2s-1", OVTK_UNIT_Litre_Squared_Per_Second);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "cmH2O %-1", OVTK_UNIT_Centimetre_Of_Water_Per_Percent);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "dyne s m-2 cm-5", OVTK_UNIT_Dyne_Seconds_Per_Square_Meter_Per_Centimetre_To_The_Power_Of_5);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mmHg %-1", OVTK_UNIT_Millimetres_Of_Mercury_Per_Percent);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "Pa %-1", OVTK_UNIT_Pascal_Per_Percent);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "B", OVTK_UNIT_Relative_Power_Decibel	);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m s-2", OVTK_UNIT_Meter_Per_Second_Squared);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "rad s2-2", OVTK_UNIT_Radians_Per_Second_Squared);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "ft min-1", OVTK_UNIT_Foot_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "in min-1", OVTK_UNIT_Inch_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "step min-1", OVTK_UNIT_Step_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "kcal", OVTK_UNIT_Kilocalories);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "rpm", OVTK_UNIT_Revolution_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "V s-1", OVTK_UNIT_V_Per_S);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "m m-1", OVTK_UNIT_M_Per_M);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "km/h", OVTK_UNIT_Velocity_Kilometer_Per_Hour);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g_s-2", OVTK_UNIT_Left_Stroke_Work_Index_Lswi);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "g_s-3", OVTK_UNIT_Indexed_Left_Cardiac_Work_Lcwi);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mHg_s-1", OVTK_UNIT_Mhg_Per_S);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "mol l-1 mm", OVTK_UNIT_Millimol_Per_Liter_X_Millimeter);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "r.p.m", OVTK_UNIT_Rotations_Per_Minute);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "dyne*s*m*m cm-5", OVTK_UNIT_Dyne_Seconds_Square_Meter_Per_Centimetre_To_The_Power_Of_5);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "l m-2", OVTK_UNIT_Litres_Per_Square_Meter);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "T", OVTK_UNIT_Tesla);
	typeManager.registerEnumerationEntry(OV_TypeId_MeasurementUnit, "deg/s", OVTK_UNIT_Degree_Per_Second);

	//  Register measurement factors
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e+24", OVTK_FACTOR_Yotta);
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e+21", OVTK_FACTOR_Zetta);
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e+18", OVTK_FACTOR_Exa);
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e+15", OVTK_FACTOR_Peta);
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e+12", OVTK_FACTOR_Tera); 
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e+09", OVTK_FACTOR_Giga);
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e+06", OVTK_FACTOR_Mega);  
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e+03", OVTK_FACTOR_Kilo);
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e+02", OVTK_FACTOR_Hecto);    
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e+01", OVTK_FACTOR_Deca);
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e+00", OVTK_FACTOR_Base);
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e-01", OVTK_FACTOR_Deci);
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e-02", OVTK_FACTOR_Centi);     
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e-03", OVTK_FACTOR_Milli);   
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e-06", OVTK_FACTOR_Micro);  
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e-09", OVTK_FACTOR_Nano);   
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e-12", OVTK_FACTOR_Pico);            
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e-15", OVTK_FACTOR_Femto);  
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e-18", OVTK_FACTOR_Atto);    
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e-21", OVTK_FACTOR_Zepto); 
	typeManager.registerEnumerationEntry(OV_TypeId_Factor, "1e-24", OVTK_FACTOR_Yocto);

	typeManager.registerEnumerationType(OVTK_TypeId_ClassificationAlgorithm, "Classification algorithm");
	typeManager.registerEnumerationType(OVTK_TypeId_ClassificationStrategy, "Classification strategy");

	// Custom Type Settings tag
	typeManager.registerEnumerationType(OVPoly_ClassId_Classifier_Algorithm, "Classifier_Algorithm");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Nearest Centroid", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Nearest Neighbors Classifier", 1);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Gaussian Naive Bayes", 2);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Stochastic Gradient Descent", 3);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Logistic Regression", 4);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Decision Tree Classifier", 5);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Extra Trees", 6);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Bagging", 7);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Random Forest", 8);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Support Vector Machine", 9);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Linear Discriminant Analysis", 10);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "AdaBoost", 11);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Multi Layer Perceptron", 12);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Riemann Minimum Distance to Mean", 13);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Riemann Tangent Space", 14);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "None", 15);

	typeManager.registerEnumerationType(OVPoly_ClassId_Knn_Algorithm, "Knn_Algorithm");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "auto", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "ball_tree", 1);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "kd_tree", 2);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "brute", 3);
	typeManager.registerEnumerationType(OVPoly_ClassId_Knn_Weights, "Knn_Weights");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Knn_Weights, "uniform", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Knn_Weights, "distance", 1);

	typeManager.registerEnumerationType(OVPoly_ClassId_Metric, "Metric");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "cityblock", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "cosine", 1);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "euclidean", 2);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "l1", 3);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "l2", 4);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "manhattan", 5);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "braycurtis", 6);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "canberra", 7);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "chebyshev", 8);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "correlation", 9);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "dice", 10);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "hamming", 11);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "jaccard", 12);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "kulsinski", 13);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "minkowski", 14);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "mahalanobis", 15);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "rogerstanimoto", 16);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "russellrao", 17);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "seuclidean", 18);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "sokalmichener", 19);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "sokalsneath", 20);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "sqeuclidean", 21);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Metric, "yule", 22);

	typeManager.registerEnumerationType(OVPoly_ClassId_Penalty, "Penalty");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Penalty, "l1", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Penalty, "l2", 1);

	typeManager.registerEnumerationType(OVPoly_ClassId_SVM_Loss, "SVM_Loss");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_SVM_Loss, "hinge", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_SVM_Loss, "squared_hinge", 1);

	typeManager.registerEnumerationType(OVPoly_ClassId_SVM_MultiClass, "SVM_MultiClass");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_SVM_MultiClass, "ovr", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_SVM_MultiClass, "crammer_singer", 1);

	typeManager.registerEnumerationType(OVPoly_ClassId_Criterion, "Criterion");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Criterion, "gini", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Criterion, "entropy", 1);


	// Logistic regression
	typeManager.registerEnumerationType(OVPoly_ClassId_Log_reg_solver, "Solver");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "newton-cg", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "lbfgs", 1);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "liblinear", 2);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "sag", 3);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "saga", 4);

	typeManager.registerEnumerationType(OVPoly_ClassId_Log_reg_multi_class, "Multi_class");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Log_reg_multi_class, "auto", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Log_reg_multi_class, "ovr", 1);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_Log_reg_multi_class, "multinominal", 2);


	// Decision Tree Classifier
	typeManager.registerEnumerationType(OVPoly_ClassId_DecisionTree_splitter, "Splitter");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_DecisionTree_splitter, "best", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_DecisionTree_splitter, "random", 1);

	// MLP 
	typeManager.registerEnumerationType(OVPoly_ClassId_MLP_activation, "Activation");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "identity", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "logistic", 1);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "tanh", 2);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "relu", 3);

	typeManager.registerEnumerationType(OVPoly_ClassId_MLP_solver, "Solver");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_MLP_solver, "lbfgs", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_MLP_solver, "sgd", 1);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_MLP_solver, "adam", 2);

	typeManager.registerEnumerationType(OVPoly_ClassId_MLP_learning_rate, "Learning rate");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_MLP_learning_rate, "constant", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_MLP_learning_rate, "invscaling", 1);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_MLP_learning_rate, "adaptive", 2);

	// SGD
	typeManager.registerEnumerationType(OVPoly_ClassId_SGD_loss, "Loss");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "hinge", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "log", 1);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "modified_huber", 2);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "squared_hinge", 3);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "perceptron", 4);

	typeManager.registerEnumerationType(OVPoly_ClassId_SGD_learning_rate, "Learning rate");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "optimal", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "constant", 1);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "invscaling", 2);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "adaptive", 3);

	// LDA
	typeManager.registerEnumerationType(OVPoly_ClassId_LDA_solver, "Loss");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_LDA_solver, "svd", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_LDA_solver, "lsqr", 1);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_LDA_solver, "eigen", 2);

	// ADA
	typeManager.registerEnumerationType(OVPoly_ClassId_ADA_algorithm, "Algorithm");
	typeManager.registerEnumerationEntry(OVPoly_ClassId_ADA_algorithm, "SAMME", 0);
	typeManager.registerEnumerationEntry(OVPoly_ClassId_ADA_algorithm, "SAMME.R", 1);


	return true;
}

bool Toolkit::uninitialize(const IKernelContext& /*ctx*/) { return true; }



