#ifndef COLORS_HEADER

struct ANSI {
    struct {
        const char *black;
        const char *white;
        const char *red;
        const char *green;
        const char *yellow;
        const char *blue;
        const char *purple;
        const char *cyan;
    } Fore;

    struct {
        const char *black;
        const char *white;
        const char *red;
        const char *green;
        const char *yellow;
        const char *blue;
        const char *purple;
        const char *cyan;
    } HFore;

    struct {
        const char *black;
        const char *white;
        const char *red;
        const char *green;
        const char *yellow;
        const char *blue;
        const char *purple;
        const char *cyan;
    } Back;

    struct {
        const char *bold;
        const char *italic;
        const char *underline;
        const char *strike;
        const char *dimmed;
        const char *reset;
    } Styles;
} ANSI;

void __ansi_setup();

#ifdef COLORS_IMPLEMENTATION
void __ansi_setup(){
    ANSI.Fore.black = "\e[30m";
    ANSI.Fore.white = "\e[37m";
    ANSI.Fore.red = "\e[31m";
    ANSI.Fore.green = "\e[32m";
    ANSI.Fore.yellow = "\e[33m";
    ANSI.Fore.blue = "\e[34m";
    ANSI.Fore.purple = "\e[35m";
    ANSI.Fore.cyan = "\e[36m";

    ANSI.HFore.black = "\e[90m";
    ANSI.HFore.white = "\e[97m";
    ANSI.HFore.red = "\e[91m";
    ANSI.HFore.green = "\e[92m";
    ANSI.HFore.yellow = "\e[93m";
    ANSI.HFore.blue = "\e[94m";
    ANSI.HFore.purple = "\e[95m";
    ANSI.HFore.cyan = "\e[96m";

    ANSI.Back.black = "\e[40m";
    ANSI.Back.white = "\e[47m";
    ANSI.Back.red = "\e[41m";
    ANSI.Back.green = "\e[42m";
    ANSI.Back.yellow = "\e[43m";
    ANSI.Back.blue = "\e[44m";
    ANSI.Back.purple = "\e[45m";
    ANSI.Back.cyan = "\e[46m";

    ANSI.Styles.bold = "\e[1m";
    ANSI.Styles.italic = "\e[3m";
    ANSI.Styles.strike = "\e[9m";
    ANSI.Styles.underline = "\e[4m";
    ANSI.Styles.dimmed = "\e[2m";
    ANSI.Styles.reset = "\e[0m";
}

#endif
#endif
#define COLORS_HEADER
