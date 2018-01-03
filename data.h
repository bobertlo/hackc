#define A_DEST_MASK 0x0020
#define M_DEST_MASK 0x0008
#define D_DEST_MASK 0x0010

typedef struct cmd {
	char *key;
	uint16_t val;
} stringmap;

stringmap comptab[] = {
	{"0",   0xEA80},
	{"1",   0xEFC0},
	{"-1",  0XEE80},
	{"D",   0xE300},
	{"A",   0xEC00},
	{"M",   0xFC00},
	{"!D",  0xE340},
	{"!A",  0xEC40},
	{"!M",  0xFC40},
	{"-D",  0xE3A0},
	{"-A",  0xEAA0},
	{"-M",  0xFAA0},
	{"D+1", 0xE7C0},
	{"A+1", 0xEDC0},
	{"M+1", 0xFDC0},
	{"D-1", 0xE380},
	{"A-1", 0xEC80},
	{"M-1", 0xFC80},
	{"D+A", 0xE080},
	{"D+M", 0xF080},
	{"D-A", 0xE4C0},
	{"D-M", 0xF4C0},
	{"A-D", 0xE1C0},
	{"M-D", 0xF1C0},
	{"D&A", 0xE000},
	{"D&M", 0xF000},
	{"D|A", 0xE540},
	{"D|M", 0xF540},
	{NULL, 0}
};

stringmap jumptab[] = {
	{"JGT", 0x0001},
	{"JEQ", 0x0002},
	{"JGE", 0x0003},
	{"JLT", 0x0004},
	{"JNE", 0x0005},
	{"JLE", 0x0006},
	{"JMP", 0x0007},
};
