module
segment2pixel(s, p);
input [27:0] s;
output [1343:0] p;
assign p[0] = 0;
assign p[1] = 0;
assign p[2] = 0;
assign p[186] = s[0];
assign p[194] = s[0];
assign p[200] = s[14];
assign p[208] = s[14];
assign p[242] = s[0];
assign p[250] = s[0];
assign p[256] = s[14];
assign p[264] = s[14];
assign p[299] = s[2];
assign p[305] = s[4];
assign p[313] = s[16];
assign p[319] = s[18];
assign p[416] = s[4];
assign p[430] = s[18];
assign p[525] = s[2];
assign p[527] = s[4];
assign p[539] = s[16];
assign p[541] = s[18];
assign p[581] = s[2];
assign p[583] = s[4];
assign p[595] = s[16];
assign p[597] = s[18];
assign p[633] = s[1];
assign p[634] = s[6];
assign p[638] = s[3];
assign p[642] = s[7];
assign p[643] = s[5];
assign p[647] = s[15];
assign p[648] = s[20];
assign p[652] = s[17];
assign p[656] = s[21];
assign p[657] = s[19];
assign p[689] = s[8];
assign p[690] = s[6];
assign p[691] = s[6];
assign p[692] = s[6];
assign p[693] = s[6];
assign p[694] = s[10];
assign p[698] = s[7];
assign p[699] = s[12];
assign p[703] = s[22];
assign p[704] = s[20];
assign p[705] = s[20];
assign p[706] = s[20];
assign p[707] = s[20];
assign p[708] = s[24];
assign p[712] = s[21];
assign p[713] = s[26];
assign p[748] = s[9];
assign p[749] = s[9];
assign p[751] = s[11];
assign p[762] = s[23];
assign p[763] = s[23];
assign p[765] = s[25];
assign p[805] = s[9];
assign p[807] = s[11];
assign p[819] = s[23];
assign p[821] = s[25];
assign p[859] = s[9];
assign p[865] = s[11];
assign p[873] = s[23];
assign p[879] = s[25];
assign p[916] = s[9];
assign p[920] = s[11];
assign p[930] = s[23];
assign p[934] = s[25];
assign p[1027] = s[9];
assign p[1033] = s[11];
assign p[1041] = s[23];
assign p[1047] = s[25];
assign p[1082] = s[13];
assign p[1090] = s[13];
assign p[1096] = s[27];
assign p[1104] = s[27];
assign p[1138] = s[13];
assign p[1139] = s[13];
assign p[1140] = s[13];
assign p[1141] = s[13];
assign p[1142] = s[13];
assign p[1143] = s[13];
assign p[1144] = s[13];
assign p[1145] = s[13];
assign p[1146] = s[13];
assign p[1152] = s[27];
assign p[1153] = s[27];
assign p[1154] = s[27];
assign p[1155] = s[27];
assign p[1156] = s[27];
assign p[1157] = s[27];
assign p[1158] = s[27];
assign p[1159] = s[27];
assign p[1160] = s[27];
endmodule