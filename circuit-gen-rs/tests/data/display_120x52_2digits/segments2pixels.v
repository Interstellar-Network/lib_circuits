// module to convert segments into an image bitmap
// generated by lib_circuits/src/segments2pixels/segments2pixels.cpp
// (c) Interstellar

module segment2pixel(s, p);  // convert segments to pixels
input [13:0] s; // segments to display
output [6239:0] p;  // pixels output
assign p[749:0] = 0;
assign p[750] = s[1];
assign p[751:751] = 0;
assign p[752] = s[0];
assign p[753] = s[0];
assign p[754] = s[0];
assign p[755] = s[0];
assign p[756] = s[0];
assign p[757] = s[0];
assign p[758] = s[0];
assign p[759] = s[0];
assign p[760] = s[0];
assign p[761] = s[0];
assign p[762] = s[0];
assign p[763] = s[0];
assign p[764] = s[0];
assign p[765] = s[0];
assign p[766] = s[0];
assign p[767] = s[0];
assign p[768] = s[0];
assign p[769] = s[0];
assign p[770] = s[0];
assign p[771] = s[0];
assign p[772] = s[0];
assign p[773:773] = 0;
assign p[774] = s[1];
assign p[785:775] = 0;
assign p[786] = s[8];
assign p[787:787] = 0;
assign p[788] = s[7];
assign p[789] = s[7];
assign p[790] = s[7];
assign p[791] = s[7];
assign p[792] = s[7];
assign p[793] = s[7];
assign p[794] = s[7];
assign p[795] = s[7];
assign p[796] = s[7];
assign p[797] = s[7];
assign p[798] = s[7];
assign p[799] = s[7];
assign p[800] = s[7];
assign p[801] = s[7];
assign p[802] = s[7];
assign p[803] = s[7];
assign p[804] = s[7];
assign p[805] = s[7];
assign p[806] = s[7];
assign p[807] = s[7];
assign p[808] = s[7];
assign p[809:809] = 0;
assign p[810] = s[8];
assign p[869:811] = 0;
assign p[870] = s[1];
assign p[871] = s[1];
assign p[872:872] = 0;
assign p[873] = s[0];
assign p[874] = s[0];
assign p[875] = s[0];
assign p[876] = s[0];
assign p[877] = s[0];
assign p[878] = s[0];
assign p[879] = s[0];
assign p[880] = s[0];
assign p[881] = s[0];
assign p[882] = s[0];
assign p[883] = s[0];
assign p[884] = s[0];
assign p[885] = s[0];
assign p[886] = s[0];
assign p[887] = s[0];
assign p[888] = s[0];
assign p[889] = s[0];
assign p[890] = s[0];
assign p[891] = s[0];
assign p[892:892] = 0;
assign p[893] = s[2];
assign p[894] = s[1];
assign p[905:895] = 0;
assign p[906] = s[8];
assign p[907] = s[8];
assign p[908:908] = 0;
assign p[909] = s[7];
assign p[910] = s[7];
assign p[911] = s[7];
assign p[912] = s[7];
assign p[913] = s[7];
assign p[914] = s[7];
assign p[915] = s[7];
assign p[916] = s[7];
assign p[917] = s[7];
assign p[918] = s[7];
assign p[919] = s[7];
assign p[920] = s[7];
assign p[921] = s[7];
assign p[922] = s[7];
assign p[923] = s[7];
assign p[924] = s[7];
assign p[925] = s[7];
assign p[926] = s[7];
assign p[927] = s[7];
assign p[928:928] = 0;
assign p[929] = s[9];
assign p[930] = s[8];
assign p[989:931] = 0;
assign p[990] = s[1];
assign p[991] = s[1];
assign p[992] = s[1];
assign p[993:993] = 0;
assign p[994] = s[0];
assign p[995] = s[0];
assign p[996] = s[0];
assign p[997] = s[0];
assign p[998] = s[0];
assign p[999] = s[0];
assign p[1000] = s[0];
assign p[1001] = s[0];
assign p[1002] = s[0];
assign p[1003] = s[0];
assign p[1004] = s[0];
assign p[1005] = s[0];
assign p[1006] = s[0];
assign p[1007] = s[0];
assign p[1008] = s[0];
assign p[1009] = s[0];
assign p[1010] = s[0];
assign p[1011:1011] = 0;
assign p[1012] = s[2];
assign p[1013] = s[2];
assign p[1014] = s[1];
assign p[1025:1015] = 0;
assign p[1026] = s[8];
assign p[1027] = s[8];
assign p[1028] = s[8];
assign p[1029:1029] = 0;
assign p[1030] = s[7];
assign p[1031] = s[7];
assign p[1032] = s[7];
assign p[1033] = s[7];
assign p[1034] = s[7];
assign p[1035] = s[7];
assign p[1036] = s[7];
assign p[1037] = s[7];
assign p[1038] = s[7];
assign p[1039] = s[7];
assign p[1040] = s[7];
assign p[1041] = s[7];
assign p[1042] = s[7];
assign p[1043] = s[7];
assign p[1044] = s[7];
assign p[1045] = s[7];
assign p[1046] = s[7];
assign p[1047:1047] = 0;
assign p[1048] = s[9];
assign p[1049] = s[9];
assign p[1050] = s[8];
assign p[1109:1051] = 0;
assign p[1110] = s[1];
assign p[1111] = s[1];
assign p[1112] = s[1];
assign p[1113] = s[1];
assign p[1114:1114] = 0;
assign p[1115] = s[0];
assign p[1116] = s[0];
assign p[1117] = s[0];
assign p[1118] = s[0];
assign p[1119] = s[0];
assign p[1120] = s[0];
assign p[1121] = s[0];
assign p[1122] = s[0];
assign p[1123] = s[0];
assign p[1124] = s[0];
assign p[1125] = s[0];
assign p[1126] = s[0];
assign p[1127] = s[0];
assign p[1128] = s[0];
assign p[1129] = s[0];
assign p[1130:1130] = 0;
assign p[1131] = s[2];
assign p[1132] = s[2];
assign p[1133] = s[2];
assign p[1134] = s[1];
assign p[1145:1135] = 0;
assign p[1146] = s[8];
assign p[1147] = s[8];
assign p[1148] = s[8];
assign p[1149] = s[8];
assign p[1150:1150] = 0;
assign p[1151] = s[7];
assign p[1152] = s[7];
assign p[1153] = s[7];
assign p[1154] = s[7];
assign p[1155] = s[7];
assign p[1156] = s[7];
assign p[1157] = s[7];
assign p[1158] = s[7];
assign p[1159] = s[7];
assign p[1160] = s[7];
assign p[1161] = s[7];
assign p[1162] = s[7];
assign p[1163] = s[7];
assign p[1164] = s[7];
assign p[1165] = s[7];
assign p[1166:1166] = 0;
assign p[1167] = s[9];
assign p[1168] = s[9];
assign p[1169] = s[9];
assign p[1170] = s[8];
assign p[1229:1171] = 0;
assign p[1230] = s[1];
assign p[1231] = s[1];
assign p[1232] = s[1];
assign p[1233] = s[1];
assign p[1234] = s[1];
assign p[1249:1235] = 0;
assign p[1250] = s[2];
assign p[1251] = s[2];
assign p[1252] = s[2];
assign p[1253] = s[2];
assign p[1254] = s[1];
assign p[1265:1255] = 0;
assign p[1266] = s[8];
assign p[1267] = s[8];
assign p[1268] = s[8];
assign p[1269] = s[8];
assign p[1270] = s[8];
assign p[1285:1271] = 0;
assign p[1286] = s[9];
assign p[1287] = s[9];
assign p[1288] = s[9];
assign p[1289] = s[9];
assign p[1290] = s[8];
assign p[1349:1291] = 0;
assign p[1350] = s[1];
assign p[1351] = s[1];
assign p[1352] = s[1];
assign p[1353] = s[1];
assign p[1354] = s[1];
assign p[1369:1355] = 0;
assign p[1370] = s[2];
assign p[1371] = s[2];
assign p[1372] = s[2];
assign p[1373] = s[2];
assign p[1374] = s[1];
assign p[1385:1375] = 0;
assign p[1386] = s[8];
assign p[1387] = s[8];
assign p[1388] = s[8];
assign p[1389] = s[8];
assign p[1390] = s[8];
assign p[1405:1391] = 0;
assign p[1406] = s[9];
assign p[1407] = s[9];
assign p[1408] = s[9];
assign p[1409] = s[9];
assign p[1410] = s[8];
assign p[1469:1411] = 0;
assign p[1470] = s[1];
assign p[1471] = s[1];
assign p[1472] = s[1];
assign p[1473] = s[1];
assign p[1474] = s[1];
assign p[1489:1475] = 0;
assign p[1490] = s[2];
assign p[1491] = s[2];
assign p[1492] = s[2];
assign p[1493] = s[2];
assign p[1494] = s[1];
assign p[1505:1495] = 0;
assign p[1506] = s[8];
assign p[1507] = s[8];
assign p[1508] = s[8];
assign p[1509] = s[8];
assign p[1510] = s[8];
assign p[1525:1511] = 0;
assign p[1526] = s[9];
assign p[1527] = s[9];
assign p[1528] = s[9];
assign p[1529] = s[9];
assign p[1530] = s[8];
assign p[1589:1531] = 0;
assign p[1590] = s[1];
assign p[1591] = s[1];
assign p[1592] = s[1];
assign p[1593] = s[1];
assign p[1594] = s[1];
assign p[1609:1595] = 0;
assign p[1610] = s[2];
assign p[1611] = s[2];
assign p[1612] = s[2];
assign p[1613] = s[2];
assign p[1614] = s[1];
assign p[1625:1615] = 0;
assign p[1626] = s[8];
assign p[1627] = s[8];
assign p[1628] = s[8];
assign p[1629] = s[8];
assign p[1630] = s[8];
assign p[1645:1631] = 0;
assign p[1646] = s[9];
assign p[1647] = s[9];
assign p[1648] = s[9];
assign p[1649] = s[9];
assign p[1650] = s[8];
assign p[1709:1651] = 0;
assign p[1710] = s[1];
assign p[1711] = s[1];
assign p[1712] = s[1];
assign p[1713] = s[1];
assign p[1714] = s[1];
assign p[1729:1715] = 0;
assign p[1730] = s[2];
assign p[1731] = s[2];
assign p[1732] = s[2];
assign p[1733] = s[2];
assign p[1734] = s[1];
assign p[1745:1735] = 0;
assign p[1746] = s[8];
assign p[1747] = s[8];
assign p[1748] = s[8];
assign p[1749] = s[8];
assign p[1750] = s[8];
assign p[1765:1751] = 0;
assign p[1766] = s[9];
assign p[1767] = s[9];
assign p[1768] = s[9];
assign p[1769] = s[9];
assign p[1770] = s[8];
assign p[1829:1771] = 0;
assign p[1830] = s[1];
assign p[1831] = s[1];
assign p[1832] = s[1];
assign p[1833] = s[1];
assign p[1834] = s[1];
assign p[1849:1835] = 0;
assign p[1850] = s[2];
assign p[1851] = s[2];
assign p[1852] = s[2];
assign p[1853] = s[2];
assign p[1854] = s[1];
assign p[1865:1855] = 0;
assign p[1866] = s[8];
assign p[1867] = s[8];
assign p[1868] = s[8];
assign p[1869] = s[8];
assign p[1870] = s[8];
assign p[1885:1871] = 0;
assign p[1886] = s[9];
assign p[1887] = s[9];
assign p[1888] = s[9];
assign p[1889] = s[9];
assign p[1890] = s[8];
assign p[1949:1891] = 0;
assign p[1950] = s[1];
assign p[1951] = s[1];
assign p[1952] = s[1];
assign p[1953] = s[1];
assign p[1954] = s[1];
assign p[1969:1955] = 0;
assign p[1970] = s[2];
assign p[1971] = s[2];
assign p[1972] = s[2];
assign p[1973] = s[2];
assign p[1974] = s[1];
assign p[1985:1975] = 0;
assign p[1986] = s[8];
assign p[1987] = s[8];
assign p[1988] = s[8];
assign p[1989] = s[8];
assign p[1990] = s[8];
assign p[2005:1991] = 0;
assign p[2006] = s[9];
assign p[2007] = s[9];
assign p[2008] = s[9];
assign p[2009] = s[9];
assign p[2010] = s[8];
assign p[2069:2011] = 0;
assign p[2070] = s[1];
assign p[2071] = s[1];
assign p[2072] = s[1];
assign p[2073] = s[1];
assign p[2074] = s[1];
assign p[2089:2075] = 0;
assign p[2090] = s[2];
assign p[2091] = s[2];
assign p[2092] = s[2];
assign p[2093] = s[2];
assign p[2094] = s[1];
assign p[2105:2095] = 0;
assign p[2106] = s[8];
assign p[2107] = s[8];
assign p[2108] = s[8];
assign p[2109] = s[8];
assign p[2110] = s[8];
assign p[2125:2111] = 0;
assign p[2126] = s[9];
assign p[2127] = s[9];
assign p[2128] = s[9];
assign p[2129] = s[9];
assign p[2130] = s[8];
assign p[2189:2131] = 0;
assign p[2190] = s[1];
assign p[2191] = s[1];
assign p[2192] = s[1];
assign p[2193] = s[1];
assign p[2194] = s[1];
assign p[2209:2195] = 0;
assign p[2210] = s[2];
assign p[2211] = s[2];
assign p[2212] = s[2];
assign p[2213] = s[2];
assign p[2214] = s[1];
assign p[2225:2215] = 0;
assign p[2226] = s[8];
assign p[2227] = s[8];
assign p[2228] = s[8];
assign p[2229] = s[8];
assign p[2230] = s[8];
assign p[2245:2231] = 0;
assign p[2246] = s[9];
assign p[2247] = s[9];
assign p[2248] = s[9];
assign p[2249] = s[9];
assign p[2250] = s[8];
assign p[2309:2251] = 0;
assign p[2310] = s[1];
assign p[2311] = s[1];
assign p[2312] = s[1];
assign p[2313] = s[1];
assign p[2314] = s[1];
assign p[2329:2315] = 0;
assign p[2330] = s[2];
assign p[2331] = s[2];
assign p[2332] = s[2];
assign p[2333] = s[2];
assign p[2334] = s[1];
assign p[2345:2335] = 0;
assign p[2346] = s[8];
assign p[2347] = s[8];
assign p[2348] = s[8];
assign p[2349] = s[8];
assign p[2350] = s[8];
assign p[2365:2351] = 0;
assign p[2366] = s[9];
assign p[2367] = s[9];
assign p[2368] = s[9];
assign p[2369] = s[9];
assign p[2370] = s[8];
assign p[2429:2371] = 0;
assign p[2430] = s[1];
assign p[2431] = s[1];
assign p[2432] = s[1];
assign p[2433] = s[1];
assign p[2434] = s[1];
assign p[2449:2435] = 0;
assign p[2450] = s[2];
assign p[2451] = s[2];
assign p[2452] = s[2];
assign p[2453] = s[2];
assign p[2454] = s[1];
assign p[2465:2455] = 0;
assign p[2466] = s[8];
assign p[2467] = s[8];
assign p[2468] = s[8];
assign p[2469] = s[8];
assign p[2470] = s[8];
assign p[2485:2471] = 0;
assign p[2486] = s[9];
assign p[2487] = s[9];
assign p[2488] = s[9];
assign p[2489] = s[9];
assign p[2490] = s[8];
assign p[2549:2491] = 0;
assign p[2550] = s[1];
assign p[2551] = s[1];
assign p[2552] = s[1];
assign p[2553] = s[1];
assign p[2554] = s[1];
assign p[2569:2555] = 0;
assign p[2570] = s[2];
assign p[2571] = s[2];
assign p[2572] = s[2];
assign p[2573] = s[2];
assign p[2574] = s[1];
assign p[2585:2575] = 0;
assign p[2586] = s[8];
assign p[2587] = s[8];
assign p[2588] = s[8];
assign p[2589] = s[8];
assign p[2590] = s[8];
assign p[2605:2591] = 0;
assign p[2606] = s[9];
assign p[2607] = s[9];
assign p[2608] = s[9];
assign p[2609] = s[9];
assign p[2610] = s[8];
assign p[2669:2611] = 0;
assign p[2670] = s[1];
assign p[2671] = s[1];
assign p[2672] = s[1];
assign p[2673] = s[1];
assign p[2674] = s[1];
assign p[2689:2675] = 0;
assign p[2690] = s[2];
assign p[2691] = s[2];
assign p[2692] = s[2];
assign p[2693] = s[2];
assign p[2694] = s[1];
assign p[2705:2695] = 0;
assign p[2706] = s[8];
assign p[2707] = s[8];
assign p[2708] = s[8];
assign p[2709] = s[8];
assign p[2710] = s[8];
assign p[2725:2711] = 0;
assign p[2726] = s[9];
assign p[2727] = s[9];
assign p[2728] = s[9];
assign p[2729] = s[9];
assign p[2730] = s[8];
assign p[2789:2731] = 0;
assign p[2790] = s[1];
assign p[2791] = s[1];
assign p[2792] = s[1];
assign p[2793] = s[1];
assign p[2810:2794] = 0;
assign p[2811] = s[2];
assign p[2812] = s[2];
assign p[2813] = s[2];
assign p[2814] = s[1];
assign p[2825:2815] = 0;
assign p[2826] = s[8];
assign p[2827] = s[8];
assign p[2828] = s[8];
assign p[2829] = s[8];
assign p[2846:2830] = 0;
assign p[2847] = s[9];
assign p[2848] = s[9];
assign p[2849] = s[9];
assign p[2850] = s[8];
assign p[2910:2851] = 0;
assign p[2911] = s[1];
assign p[2912] = s[1];
assign p[2914:2913] = 0;
assign p[2915] = s[3];
assign p[2916] = s[3];
assign p[2917] = s[3];
assign p[2918] = s[3];
assign p[2919] = s[3];
assign p[2920] = s[3];
assign p[2921] = s[3];
assign p[2922] = s[3];
assign p[2923] = s[3];
assign p[2924] = s[3];
assign p[2925] = s[3];
assign p[2926] = s[3];
assign p[2927] = s[3];
assign p[2928] = s[3];
assign p[2929] = s[3];
assign p[2931:2930] = 0;
assign p[2932] = s[2];
assign p[2933] = s[2];
assign p[2946:2934] = 0;
assign p[2947] = s[8];
assign p[2948] = s[8];
assign p[2950:2949] = 0;
assign p[2951] = s[10];
assign p[2952] = s[10];
assign p[2953] = s[10];
assign p[2954] = s[10];
assign p[2955] = s[10];
assign p[2956] = s[10];
assign p[2957] = s[10];
assign p[2958] = s[10];
assign p[2959] = s[10];
assign p[2960] = s[10];
assign p[2961] = s[10];
assign p[2962] = s[10];
assign p[2963] = s[10];
assign p[2964] = s[10];
assign p[2965] = s[10];
assign p[2967:2966] = 0;
assign p[2968] = s[9];
assign p[2969] = s[9];
assign p[3033:2970] = 0;
assign p[3034] = s[3];
assign p[3035] = s[3];
assign p[3036] = s[3];
assign p[3037] = s[3];
assign p[3038] = s[3];
assign p[3039] = s[3];
assign p[3040] = s[3];
assign p[3041] = s[3];
assign p[3042] = s[3];
assign p[3043] = s[3];
assign p[3044] = s[3];
assign p[3045] = s[3];
assign p[3046] = s[3];
assign p[3047] = s[3];
assign p[3048] = s[3];
assign p[3049] = s[3];
assign p[3050] = s[3];
assign p[3069:3051] = 0;
assign p[3070] = s[10];
assign p[3071] = s[10];
assign p[3072] = s[10];
assign p[3073] = s[10];
assign p[3074] = s[10];
assign p[3075] = s[10];
assign p[3076] = s[10];
assign p[3077] = s[10];
assign p[3078] = s[10];
assign p[3079] = s[10];
assign p[3080] = s[10];
assign p[3081] = s[10];
assign p[3082] = s[10];
assign p[3083] = s[10];
assign p[3084] = s[10];
assign p[3085] = s[10];
assign p[3086] = s[10];
assign p[3152:3087] = 0;
assign p[3153] = s[3];
assign p[3154] = s[3];
assign p[3155] = s[3];
assign p[3156] = s[3];
assign p[3157] = s[3];
assign p[3158] = s[3];
assign p[3159] = s[3];
assign p[3160] = s[3];
assign p[3161] = s[3];
assign p[3162] = s[3];
assign p[3163] = s[3];
assign p[3164] = s[3];
assign p[3165] = s[3];
assign p[3166] = s[3];
assign p[3167] = s[3];
assign p[3168] = s[3];
assign p[3169] = s[3];
assign p[3170] = s[3];
assign p[3171] = s[3];
assign p[3188:3172] = 0;
assign p[3189] = s[10];
assign p[3190] = s[10];
assign p[3191] = s[10];
assign p[3192] = s[10];
assign p[3193] = s[10];
assign p[3194] = s[10];
assign p[3195] = s[10];
assign p[3196] = s[10];
assign p[3197] = s[10];
assign p[3198] = s[10];
assign p[3199] = s[10];
assign p[3200] = s[10];
assign p[3201] = s[10];
assign p[3202] = s[10];
assign p[3203] = s[10];
assign p[3204] = s[10];
assign p[3205] = s[10];
assign p[3206] = s[10];
assign p[3207] = s[10];
assign p[3273:3208] = 0;
assign p[3274] = s[3];
assign p[3275] = s[3];
assign p[3276] = s[3];
assign p[3277] = s[3];
assign p[3278] = s[3];
assign p[3279] = s[3];
assign p[3280] = s[3];
assign p[3281] = s[3];
assign p[3282] = s[3];
assign p[3283] = s[3];
assign p[3284] = s[3];
assign p[3285] = s[3];
assign p[3286] = s[3];
assign p[3287] = s[3];
assign p[3288] = s[3];
assign p[3289] = s[3];
assign p[3290] = s[3];
assign p[3309:3291] = 0;
assign p[3310] = s[10];
assign p[3311] = s[10];
assign p[3312] = s[10];
assign p[3313] = s[10];
assign p[3314] = s[10];
assign p[3315] = s[10];
assign p[3316] = s[10];
assign p[3317] = s[10];
assign p[3318] = s[10];
assign p[3319] = s[10];
assign p[3320] = s[10];
assign p[3321] = s[10];
assign p[3322] = s[10];
assign p[3323] = s[10];
assign p[3324] = s[10];
assign p[3325] = s[10];
assign p[3326] = s[10];
assign p[3390:3327] = 0;
assign p[3391] = s[4];
assign p[3392] = s[4];
assign p[3394:3393] = 0;
assign p[3395] = s[3];
assign p[3396] = s[3];
assign p[3397] = s[3];
assign p[3398] = s[3];
assign p[3399] = s[3];
assign p[3400] = s[3];
assign p[3401] = s[3];
assign p[3402] = s[3];
assign p[3403] = s[3];
assign p[3404] = s[3];
assign p[3405] = s[3];
assign p[3406] = s[3];
assign p[3407] = s[3];
assign p[3408] = s[3];
assign p[3409] = s[3];
assign p[3411:3410] = 0;
assign p[3412] = s[5];
assign p[3413] = s[5];
assign p[3426:3414] = 0;
assign p[3427] = s[11];
assign p[3428] = s[11];
assign p[3430:3429] = 0;
assign p[3431] = s[10];
assign p[3432] = s[10];
assign p[3433] = s[10];
assign p[3434] = s[10];
assign p[3435] = s[10];
assign p[3436] = s[10];
assign p[3437] = s[10];
assign p[3438] = s[10];
assign p[3439] = s[10];
assign p[3440] = s[10];
assign p[3441] = s[10];
assign p[3442] = s[10];
assign p[3443] = s[10];
assign p[3444] = s[10];
assign p[3445] = s[10];
assign p[3447:3446] = 0;
assign p[3448] = s[12];
assign p[3449] = s[12];
assign p[3509:3450] = 0;
assign p[3510] = s[4];
assign p[3511] = s[4];
assign p[3512] = s[4];
assign p[3513] = s[4];
assign p[3530:3514] = 0;
assign p[3531] = s[5];
assign p[3532] = s[5];
assign p[3533] = s[5];
assign p[3534] = s[4];
assign p[3545:3535] = 0;
assign p[3546] = s[11];
assign p[3547] = s[11];
assign p[3548] = s[11];
assign p[3549] = s[11];
assign p[3566:3550] = 0;
assign p[3567] = s[12];
assign p[3568] = s[12];
assign p[3569] = s[12];
assign p[3570] = s[11];
assign p[3629:3571] = 0;
assign p[3630] = s[4];
assign p[3631] = s[4];
assign p[3632] = s[4];
assign p[3633] = s[4];
assign p[3634] = s[4];
assign p[3649:3635] = 0;
assign p[3650] = s[5];
assign p[3651] = s[5];
assign p[3652] = s[5];
assign p[3653] = s[5];
assign p[3654] = s[4];
assign p[3665:3655] = 0;
assign p[3666] = s[11];
assign p[3667] = s[11];
assign p[3668] = s[11];
assign p[3669] = s[11];
assign p[3670] = s[11];
assign p[3685:3671] = 0;
assign p[3686] = s[12];
assign p[3687] = s[12];
assign p[3688] = s[12];
assign p[3689] = s[12];
assign p[3690] = s[11];
assign p[3749:3691] = 0;
assign p[3750] = s[4];
assign p[3751] = s[4];
assign p[3752] = s[4];
assign p[3753] = s[4];
assign p[3754] = s[4];
assign p[3769:3755] = 0;
assign p[3770] = s[5];
assign p[3771] = s[5];
assign p[3772] = s[5];
assign p[3773] = s[5];
assign p[3774] = s[4];
assign p[3785:3775] = 0;
assign p[3786] = s[11];
assign p[3787] = s[11];
assign p[3788] = s[11];
assign p[3789] = s[11];
assign p[3790] = s[11];
assign p[3805:3791] = 0;
assign p[3806] = s[12];
assign p[3807] = s[12];
assign p[3808] = s[12];
assign p[3809] = s[12];
assign p[3810] = s[11];
assign p[3869:3811] = 0;
assign p[3870] = s[4];
assign p[3871] = s[4];
assign p[3872] = s[4];
assign p[3873] = s[4];
assign p[3874] = s[4];
assign p[3889:3875] = 0;
assign p[3890] = s[5];
assign p[3891] = s[5];
assign p[3892] = s[5];
assign p[3893] = s[5];
assign p[3894] = s[4];
assign p[3905:3895] = 0;
assign p[3906] = s[11];
assign p[3907] = s[11];
assign p[3908] = s[11];
assign p[3909] = s[11];
assign p[3910] = s[11];
assign p[3925:3911] = 0;
assign p[3926] = s[12];
assign p[3927] = s[12];
assign p[3928] = s[12];
assign p[3929] = s[12];
assign p[3930] = s[11];
assign p[3989:3931] = 0;
assign p[3990] = s[4];
assign p[3991] = s[4];
assign p[3992] = s[4];
assign p[3993] = s[4];
assign p[3994] = s[4];
assign p[4009:3995] = 0;
assign p[4010] = s[5];
assign p[4011] = s[5];
assign p[4012] = s[5];
assign p[4013] = s[5];
assign p[4014] = s[4];
assign p[4025:4015] = 0;
assign p[4026] = s[11];
assign p[4027] = s[11];
assign p[4028] = s[11];
assign p[4029] = s[11];
assign p[4030] = s[11];
assign p[4045:4031] = 0;
assign p[4046] = s[12];
assign p[4047] = s[12];
assign p[4048] = s[12];
assign p[4049] = s[12];
assign p[4050] = s[11];
assign p[4109:4051] = 0;
assign p[4110] = s[4];
assign p[4111] = s[4];
assign p[4112] = s[4];
assign p[4113] = s[4];
assign p[4114] = s[4];
assign p[4129:4115] = 0;
assign p[4130] = s[5];
assign p[4131] = s[5];
assign p[4132] = s[5];
assign p[4133] = s[5];
assign p[4134] = s[4];
assign p[4145:4135] = 0;
assign p[4146] = s[11];
assign p[4147] = s[11];
assign p[4148] = s[11];
assign p[4149] = s[11];
assign p[4150] = s[11];
assign p[4165:4151] = 0;
assign p[4166] = s[12];
assign p[4167] = s[12];
assign p[4168] = s[12];
assign p[4169] = s[12];
assign p[4170] = s[11];
assign p[4229:4171] = 0;
assign p[4230] = s[4];
assign p[4231] = s[4];
assign p[4232] = s[4];
assign p[4233] = s[4];
assign p[4234] = s[4];
assign p[4249:4235] = 0;
assign p[4250] = s[5];
assign p[4251] = s[5];
assign p[4252] = s[5];
assign p[4253] = s[5];
assign p[4254] = s[4];
assign p[4265:4255] = 0;
assign p[4266] = s[11];
assign p[4267] = s[11];
assign p[4268] = s[11];
assign p[4269] = s[11];
assign p[4270] = s[11];
assign p[4285:4271] = 0;
assign p[4286] = s[12];
assign p[4287] = s[12];
assign p[4288] = s[12];
assign p[4289] = s[12];
assign p[4290] = s[11];
assign p[4349:4291] = 0;
assign p[4350] = s[4];
assign p[4351] = s[4];
assign p[4352] = s[4];
assign p[4353] = s[4];
assign p[4354] = s[4];
assign p[4369:4355] = 0;
assign p[4370] = s[5];
assign p[4371] = s[5];
assign p[4372] = s[5];
assign p[4373] = s[5];
assign p[4374] = s[4];
assign p[4385:4375] = 0;
assign p[4386] = s[11];
assign p[4387] = s[11];
assign p[4388] = s[11];
assign p[4389] = s[11];
assign p[4390] = s[11];
assign p[4405:4391] = 0;
assign p[4406] = s[12];
assign p[4407] = s[12];
assign p[4408] = s[12];
assign p[4409] = s[12];
assign p[4410] = s[11];
assign p[4469:4411] = 0;
assign p[4470] = s[4];
assign p[4471] = s[4];
assign p[4472] = s[4];
assign p[4473] = s[4];
assign p[4474] = s[4];
assign p[4489:4475] = 0;
assign p[4490] = s[5];
assign p[4491] = s[5];
assign p[4492] = s[5];
assign p[4493] = s[5];
assign p[4494] = s[4];
assign p[4505:4495] = 0;
assign p[4506] = s[11];
assign p[4507] = s[11];
assign p[4508] = s[11];
assign p[4509] = s[11];
assign p[4510] = s[11];
assign p[4525:4511] = 0;
assign p[4526] = s[12];
assign p[4527] = s[12];
assign p[4528] = s[12];
assign p[4529] = s[12];
assign p[4530] = s[11];
assign p[4589:4531] = 0;
assign p[4590] = s[4];
assign p[4591] = s[4];
assign p[4592] = s[4];
assign p[4593] = s[4];
assign p[4594] = s[4];
assign p[4609:4595] = 0;
assign p[4610] = s[5];
assign p[4611] = s[5];
assign p[4612] = s[5];
assign p[4613] = s[5];
assign p[4614] = s[4];
assign p[4625:4615] = 0;
assign p[4626] = s[11];
assign p[4627] = s[11];
assign p[4628] = s[11];
assign p[4629] = s[11];
assign p[4630] = s[11];
assign p[4645:4631] = 0;
assign p[4646] = s[12];
assign p[4647] = s[12];
assign p[4648] = s[12];
assign p[4649] = s[12];
assign p[4650] = s[11];
assign p[4709:4651] = 0;
assign p[4710] = s[4];
assign p[4711] = s[4];
assign p[4712] = s[4];
assign p[4713] = s[4];
assign p[4714] = s[4];
assign p[4729:4715] = 0;
assign p[4730] = s[5];
assign p[4731] = s[5];
assign p[4732] = s[5];
assign p[4733] = s[5];
assign p[4734] = s[4];
assign p[4745:4735] = 0;
assign p[4746] = s[11];
assign p[4747] = s[11];
assign p[4748] = s[11];
assign p[4749] = s[11];
assign p[4750] = s[11];
assign p[4765:4751] = 0;
assign p[4766] = s[12];
assign p[4767] = s[12];
assign p[4768] = s[12];
assign p[4769] = s[12];
assign p[4770] = s[11];
assign p[4829:4771] = 0;
assign p[4830] = s[4];
assign p[4831] = s[4];
assign p[4832] = s[4];
assign p[4833] = s[4];
assign p[4834] = s[4];
assign p[4849:4835] = 0;
assign p[4850] = s[5];
assign p[4851] = s[5];
assign p[4852] = s[5];
assign p[4853] = s[5];
assign p[4854] = s[4];
assign p[4865:4855] = 0;
assign p[4866] = s[11];
assign p[4867] = s[11];
assign p[4868] = s[11];
assign p[4869] = s[11];
assign p[4870] = s[11];
assign p[4885:4871] = 0;
assign p[4886] = s[12];
assign p[4887] = s[12];
assign p[4888] = s[12];
assign p[4889] = s[12];
assign p[4890] = s[11];
assign p[4949:4891] = 0;
assign p[4950] = s[4];
assign p[4951] = s[4];
assign p[4952] = s[4];
assign p[4953] = s[4];
assign p[4954] = s[4];
assign p[4969:4955] = 0;
assign p[4970] = s[5];
assign p[4971] = s[5];
assign p[4972] = s[5];
assign p[4973] = s[5];
assign p[4974] = s[4];
assign p[4985:4975] = 0;
assign p[4986] = s[11];
assign p[4987] = s[11];
assign p[4988] = s[11];
assign p[4989] = s[11];
assign p[4990] = s[11];
assign p[5005:4991] = 0;
assign p[5006] = s[12];
assign p[5007] = s[12];
assign p[5008] = s[12];
assign p[5009] = s[12];
assign p[5010] = s[11];
assign p[5069:5011] = 0;
assign p[5070] = s[4];
assign p[5071] = s[4];
assign p[5072] = s[4];
assign p[5073] = s[4];
assign p[5074] = s[4];
assign p[5089:5075] = 0;
assign p[5090] = s[5];
assign p[5091] = s[5];
assign p[5092] = s[5];
assign p[5093] = s[5];
assign p[5094] = s[4];
assign p[5105:5095] = 0;
assign p[5106] = s[11];
assign p[5107] = s[11];
assign p[5108] = s[11];
assign p[5109] = s[11];
assign p[5110] = s[11];
assign p[5125:5111] = 0;
assign p[5126] = s[12];
assign p[5127] = s[12];
assign p[5128] = s[12];
assign p[5129] = s[12];
assign p[5130] = s[11];
assign p[5189:5131] = 0;
assign p[5190] = s[4];
assign p[5191] = s[4];
assign p[5192] = s[4];
assign p[5193] = s[4];
assign p[5194:5194] = 0;
assign p[5195] = s[6];
assign p[5196] = s[6];
assign p[5197] = s[6];
assign p[5198] = s[6];
assign p[5199] = s[6];
assign p[5200] = s[6];
assign p[5201] = s[6];
assign p[5202] = s[6];
assign p[5203] = s[6];
assign p[5204] = s[6];
assign p[5205] = s[6];
assign p[5206] = s[6];
assign p[5207] = s[6];
assign p[5208] = s[6];
assign p[5209] = s[6];
assign p[5210:5210] = 0;
assign p[5211] = s[5];
assign p[5212] = s[5];
assign p[5213] = s[5];
assign p[5214] = s[4];
assign p[5225:5215] = 0;
assign p[5226] = s[11];
assign p[5227] = s[11];
assign p[5228] = s[11];
assign p[5229] = s[11];
assign p[5230:5230] = 0;
assign p[5231] = s[13];
assign p[5232] = s[13];
assign p[5233] = s[13];
assign p[5234] = s[13];
assign p[5235] = s[13];
assign p[5236] = s[13];
assign p[5237] = s[13];
assign p[5238] = s[13];
assign p[5239] = s[13];
assign p[5240] = s[13];
assign p[5241] = s[13];
assign p[5242] = s[13];
assign p[5243] = s[13];
assign p[5244] = s[13];
assign p[5245] = s[13];
assign p[5246:5246] = 0;
assign p[5247] = s[12];
assign p[5248] = s[12];
assign p[5249] = s[12];
assign p[5250] = s[11];
assign p[5309:5251] = 0;
assign p[5310] = s[4];
assign p[5311] = s[4];
assign p[5312] = s[4];
assign p[5313:5313] = 0;
assign p[5314] = s[6];
assign p[5315] = s[6];
assign p[5316] = s[6];
assign p[5317] = s[6];
assign p[5318] = s[6];
assign p[5319] = s[6];
assign p[5320] = s[6];
assign p[5321] = s[6];
assign p[5322] = s[6];
assign p[5323] = s[6];
assign p[5324] = s[6];
assign p[5325] = s[6];
assign p[5326] = s[6];
assign p[5327] = s[6];
assign p[5328] = s[6];
assign p[5329] = s[6];
assign p[5330] = s[6];
assign p[5331:5331] = 0;
assign p[5332] = s[5];
assign p[5333] = s[5];
assign p[5334] = s[4];
assign p[5345:5335] = 0;
assign p[5346] = s[11];
assign p[5347] = s[11];
assign p[5348] = s[11];
assign p[5349:5349] = 0;
assign p[5350] = s[13];
assign p[5351] = s[13];
assign p[5352] = s[13];
assign p[5353] = s[13];
assign p[5354] = s[13];
assign p[5355] = s[13];
assign p[5356] = s[13];
assign p[5357] = s[13];
assign p[5358] = s[13];
assign p[5359] = s[13];
assign p[5360] = s[13];
assign p[5361] = s[13];
assign p[5362] = s[13];
assign p[5363] = s[13];
assign p[5364] = s[13];
assign p[5365] = s[13];
assign p[5366] = s[13];
assign p[5367:5367] = 0;
assign p[5368] = s[12];
assign p[5369] = s[12];
assign p[5370] = s[11];
assign p[5429:5371] = 0;
assign p[5430] = s[4];
assign p[5431] = s[4];
assign p[5432:5432] = 0;
assign p[5433] = s[6];
assign p[5434] = s[6];
assign p[5435] = s[6];
assign p[5436] = s[6];
assign p[5437] = s[6];
assign p[5438] = s[6];
assign p[5439] = s[6];
assign p[5440] = s[6];
assign p[5441] = s[6];
assign p[5442] = s[6];
assign p[5443] = s[6];
assign p[5444] = s[6];
assign p[5445] = s[6];
assign p[5446] = s[6];
assign p[5447] = s[6];
assign p[5448] = s[6];
assign p[5449] = s[6];
assign p[5450] = s[6];
assign p[5451] = s[6];
assign p[5452:5452] = 0;
assign p[5453] = s[5];
assign p[5454] = s[4];
assign p[5465:5455] = 0;
assign p[5466] = s[11];
assign p[5467] = s[11];
assign p[5468:5468] = 0;
assign p[5469] = s[13];
assign p[5470] = s[13];
assign p[5471] = s[13];
assign p[5472] = s[13];
assign p[5473] = s[13];
assign p[5474] = s[13];
assign p[5475] = s[13];
assign p[5476] = s[13];
assign p[5477] = s[13];
assign p[5478] = s[13];
assign p[5479] = s[13];
assign p[5480] = s[13];
assign p[5481] = s[13];
assign p[5482] = s[13];
assign p[5483] = s[13];
assign p[5484] = s[13];
assign p[5485] = s[13];
assign p[5486] = s[13];
assign p[5487] = s[13];
assign p[5488:5488] = 0;
assign p[5489] = s[12];
assign p[5490] = s[11];
assign p[5549:5491] = 0;
assign p[5550] = s[4];
assign p[5551:5551] = 0;
assign p[5552] = s[6];
assign p[5553] = s[6];
assign p[5554] = s[6];
assign p[5555] = s[6];
assign p[5556] = s[6];
assign p[5557] = s[6];
assign p[5558] = s[6];
assign p[5559] = s[6];
assign p[5560] = s[6];
assign p[5561] = s[6];
assign p[5562] = s[6];
assign p[5563] = s[6];
assign p[5564] = s[6];
assign p[5565] = s[6];
assign p[5566] = s[6];
assign p[5567] = s[6];
assign p[5568] = s[6];
assign p[5569] = s[6];
assign p[5570] = s[6];
assign p[5571] = s[6];
assign p[5572] = s[6];
assign p[5573:5573] = 0;
assign p[5574] = s[4];
assign p[5585:5575] = 0;
assign p[5586] = s[11];
assign p[5587:5587] = 0;
assign p[5588] = s[13];
assign p[5589] = s[13];
assign p[5590] = s[13];
assign p[5591] = s[13];
assign p[5592] = s[13];
assign p[5593] = s[13];
assign p[5594] = s[13];
assign p[5595] = s[13];
assign p[5596] = s[13];
assign p[5597] = s[13];
assign p[5598] = s[13];
assign p[5599] = s[13];
assign p[5600] = s[13];
assign p[5601] = s[13];
assign p[5602] = s[13];
assign p[5603] = s[13];
assign p[5604] = s[13];
assign p[5605] = s[13];
assign p[5606] = s[13];
assign p[5607] = s[13];
assign p[5608] = s[13];
assign p[5609:5609] = 0;
assign p[5610] = s[11];
assign p[6239:5611] = 0;
endmodule