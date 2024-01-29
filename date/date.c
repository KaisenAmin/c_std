#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include "date.h"
#include "../fmt/fmt.h"
#include "../time/time.h"



#if defined(_WIN32) || defined(_WIN64)
static const unsigned char *conv_num(const unsigned char *, int *, unsigned int, unsigned int);
static const unsigned char *find_string(const unsigned char *, int *, const char * const *, const char * const *, int);

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E			0x01
#define ALT_O			0x02
#define LEGAL_ALT(x)	{ if (alt_format & ~(x)) return NULL; }

#define TM_YEAR_BASE	1900
#define TM_SUNDAY       0
#define TM_MONDAY       1
#define TM_TUESDAY      2
#define TM_WEDNESDAY    3
#define TM_THURSDAY     4
#define TM_FRIDAY       5
#define TM_SATURDAY     6
#define S_YEAR			(1 << 0)
#define S_MON			(1 << 1)
#define S_YDAY			(1 << 2)
#define S_MDAY			(1 << 3)
#define S_WDAY			(1 << 4)
#define S_HOUR			(1 << 5)
#define HAVE_MDAY(s)	(s & S_MDAY)
#define HAVE_MON(s)		(s & S_MON)
#define HAVE_WDAY(s)	(s & S_WDAY)
#define HAVE_YDAY(s)	(s & S_YDAY)
#define HAVE_YEAR(s)	(s & S_YEAR)
#define HAVE_HOUR(s)	(s & S_HOUR)
#define SECSPERMIN      60
#define MINSPERHOUR     60
#define SECSPERHOUR     (SECSPERMIN * MINSPERHOUR)
#define HOURSPERDAY     24
#define HERE_D_T_FMT    "%a %b %e %H:%M:%S %Y"
#define HERE_D_FMT      "%y/%m/%d"
#define HERE_T_FMT_AMPM "%I:%M:%S %p"
#define HERE_T_FMT      "%H:%M:%S"
#define isleap(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))
#define isleap_sum(a, b)	isleap((a) % 400 + (b) % 400)

#ifdef _MSC_VER
#define tzname              _tzname
#define strncasecmp         _strnicmp
#endif

#ifdef TM_ZONE
static char* utc = "UTC";
#endif
/* RFC-822/RFC-2822 */
static const char* const nast[] = {
       "EST",    "CST",    "MST",    "PST",    "\0\0\0"
};
static const char* const nadt[] = {
       "EDT",    "CDT",    "MDT",    "PDT",    "\0\0\0"
};
static const char* weekday_name[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};
static const char* ab_weekday_name[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
static const char* month_name[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};
static const char* ab_month_name[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
static const char* am_pm[] = {"AM", "PM"};
static const int start_of_month[2][13] = {
    /* non-leap year */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* leap year */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

static int first_wday_of(int yr) {
    return ((2 * (3 - (yr / 100) % 4)) + (yr % 100) + ((yr % 100) /  4) +
        (isleap(yr) ? 6 : 0) + 1) % 7;
}

#define delim(p)	((p) == '\0' || isspace((unsigned char)(p)))
#include <time.h>
static int fromzone(const unsigned char **bp, struct tm *tm, int mandatory) {
//    timezone_t tz;
    char buf[512], *p;
    const unsigned char *rp;

    for (p = buf, rp = *bp; !delim(*rp) && p < &buf[sizeof(buf) - 1]; rp++)
        *p++ = *rp;
    *p = '\0';

    if (mandatory)
        *bp = rp;
    if (!isalnum((unsigned char)*buf))
        return 0;

    *bp = rp;
    tm->tm_isdst = 0;	/* XXX */
#ifdef TM_GMTOFF
    tm->TM_GMTOFF = tzgetgmtoff(tz, tm->tm_isdst);
#endif
#ifdef TM_ZONE
    // Can't use tzgetname() here because we are going to free()
    tm->TM_ZONE = NULL; /* XXX */
#endif
//    tzfree(tz);
    return 1;
}

char* win_strptime(const char *buf, const char *fmt, struct tm *tm)
{
    unsigned char c;
    const unsigned char *bp, *ep, *zname;
    int alt_format, i, split_year = 0, neg = 0, state = 0,
        day_offset = -1, week_offset = 0, offs, mandatory;
    const char *new_fmt;

    bp = (const unsigned char *)buf;

    while (bp != NULL && (c = *fmt++) != '\0') {
        /* Clear `alternate' modifier prior to new conversion. */
        alt_format = 0;
        i = 0;

        /* Eat up white-space. */
        if (isspace(c)) {
            while (isspace(*bp))
                bp++;
            continue;
        }

        if (c != '%')
            goto literal;


again:		switch (c = *fmt++) {
        case '%':	/* "%%" is converted to "%". */
literal:
            if (c != *bp++)
                return NULL;
            LEGAL_ALT(0);
            continue;

        /*
         * "Alternative" modifiers. Just set the appropriate flag
         * and start over again.
         */
        case 'E':	/* "%E?" alternative conversion modifier. */
            LEGAL_ALT(0);
            alt_format |= ALT_E;
            goto again;

        case 'O':	/* "%O?" alternative conversion modifier. */
            LEGAL_ALT(0);
            alt_format |= ALT_O;
            goto again;

        /*
         * "Complex" conversion rules, implemented through recursion.
         */
        case 'c':	/* Date and time, using the locale's format. */
//            new_fmt = _TIME_LOCALE(loc)->d_t_fmt;
            new_fmt = HERE_D_T_FMT;
            state |= S_WDAY | S_MON | S_MDAY | S_YEAR;
            goto recurse;

        case 'F':	/* The date as "%Y-%m-%d". */
            new_fmt = "%Y-%m-%d";
            LEGAL_ALT(0);
            state |= S_MON | S_MDAY | S_YEAR;
            goto recurse;

        case 'R':	/* The time as "%H:%M". */
            new_fmt = "%H:%M";
            LEGAL_ALT(0);
            goto recurse;

        case 'r':	/* The time in 12-hour clock representation. */
//            new_fmt = _TIME_LOCALE(loc)->t_fmt_ampm;
            new_fmt = HERE_T_FMT_AMPM;
            LEGAL_ALT(0);
            goto recurse;

        case 'X':	/* The time, using the locale's format. */
            /* fall through */

        case 'T':	/* The time as "%H:%M:%S". */
            new_fmt = HERE_T_FMT;
            LEGAL_ALT(0);

recurse:
            bp = (const unsigned char *)win_strptime((const char *)bp,
                                new_fmt, tm);
            LEGAL_ALT(ALT_E);
            continue;

        case 'x':	/* The date, using the locale's format. */
            /* fall throug */

        case 'D':	/* The date as "%y/%m/%d". */
        {
            new_fmt = HERE_D_FMT;
            LEGAL_ALT(0);
            state |= S_MON | S_MDAY | S_YEAR;
            const int year = split_year ? tm->tm_year : 0;

            bp = (const unsigned char *)win_strptime((const char *)bp,
                                new_fmt, tm);
            LEGAL_ALT(ALT_E);
            tm->tm_year += year;
            if (split_year && tm->tm_year % (2000 - TM_YEAR_BASE) <= 68)
                tm->tm_year -= 2000 - TM_YEAR_BASE;
            split_year = 1;
            continue;
        }
        /*
         * "Elementary" conversion rules.
         */
        case 'A':	/* The day of week, using the locale's form. */
        case 'a':
            bp = find_string(bp, &tm->tm_wday, weekday_name, ab_weekday_name, 7);
            LEGAL_ALT(0);
            state |= S_WDAY;
            continue;

        case 'B':	/* The month, using the locale's form. */
        case 'b':
        case 'h':
            bp = find_string(bp, &tm->tm_mon, month_name, ab_month_name, 12);
            LEGAL_ALT(0);
            state |= S_MON;
            continue;

        case 'C':	/* The century number. */
            i = 20;
            bp = conv_num(bp, &i, 0, 99);

            i = i * 100 - TM_YEAR_BASE;
            if (split_year)
                i += tm->tm_year % 100;
            split_year = 1;
            tm->tm_year = i;
            LEGAL_ALT(ALT_E);
            state |= S_YEAR;
            continue;

        case 'd':	/* The day of month. */
        case 'e':
            bp = conv_num(bp, &tm->tm_mday, 1, 31);
            LEGAL_ALT(ALT_O);
            state |= S_MDAY;
            continue;

        case 'k':	/* The hour (24-hour clock representation). */
            LEGAL_ALT(0);
            /* FALLTHROUGH */
        case 'H':
            bp = conv_num(bp, &tm->tm_hour, 0, 23);
            LEGAL_ALT(ALT_O);
            state |= S_HOUR;
            continue;

        case 'l':	/* The hour (12-hour clock representation). */
            LEGAL_ALT(0);
            /* FALLTHROUGH */
        case 'I':
            bp = conv_num(bp, &tm->tm_hour, 1, 12);
            if (tm->tm_hour == 12)
                tm->tm_hour = 0;
            LEGAL_ALT(ALT_O);
            state |= S_HOUR;
            continue;

        case 'j':	/* The day of year. */
            i = 1;
            bp = conv_num(bp, &i, 1, 366);
            tm->tm_yday = i - 1;
            LEGAL_ALT(0);
            state |= S_YDAY;
            continue;

        case 'M':	/* The minute. */
            bp = conv_num(bp, &tm->tm_min, 0, 59);
            LEGAL_ALT(ALT_O);
            continue;

        case 'm':	/* The month. */
            i = 1;
            bp = conv_num(bp, &i, 1, 12);
            tm->tm_mon = i - 1;
            LEGAL_ALT(ALT_O);
            state |= S_MON;
            continue;

        case 'p':	/* The locale's equivalent of AM/PM. */
            bp = find_string(bp, &i, am_pm, NULL, 2);
            if (HAVE_HOUR(state) && tm->tm_hour > 11)
                return NULL;
            tm->tm_hour += i * 12;
            LEGAL_ALT(0);
            continue;

        case 'S':	/* The seconds. */
            bp = conv_num(bp, &tm->tm_sec, 0, 61);
            LEGAL_ALT(ALT_O);
            continue;

#ifndef TIME_MAX
#define TIME_MAX	INT64_MAX
#endif
        case 's':	/* seconds since the epoch */
            {
                time_t sse = 0;
                uint64_t rulim = TIME_MAX;

                if (*bp < '0' || *bp > '9') {
                    bp = NULL;
                    continue;
                }

                do {
                    sse *= 10;
                    sse += *bp++ - '0';
                    rulim /= 10;
                } while ((sse * 10 <= TIME_MAX) &&
                     rulim && *bp >= '0' && *bp <= '9');

                if (sse < 0 || (uint64_t)sse > TIME_MAX) {
                    bp = NULL;
                    continue;
                }
#ifdef _WIN32
                if (localtime_s(tm, &sse) == 0)
#else
                if (localtime_r(&sse, tm))
#endif
                    state |= S_YDAY | S_WDAY | S_MON | S_MDAY | S_YEAR;
                else
                    bp = NULL;
            }
            continue;

        case 'U':	/* The week of year, beginning on sunday. */
        case 'W':	/* The week of year, beginning on monday. */
            bp = conv_num(bp, &i, 0, 53);
            LEGAL_ALT(ALT_O);
            if (c == 'U')
                day_offset = TM_SUNDAY;
            else
                day_offset = TM_MONDAY;
            week_offset = i;
            continue;

        case 'w':	/* The day of week, beginning on sunday. */
            bp = conv_num(bp, &tm->tm_wday, 0, 6);
            LEGAL_ALT(ALT_O);
            state |= S_WDAY;
            continue;

        case 'u':	/* The day of week, monday = 1. */
            bp = conv_num(bp, &i, 1, 7);
            tm->tm_wday = i % 7;
            LEGAL_ALT(ALT_O);
            state |= S_WDAY;
            continue;

        case 'g':	/* The year corresponding to the ISO week
                 * number but without the century.
                 */
            bp = conv_num(bp, &i, 0, 99);
            continue;

        case 'G':	/* The year corresponding to the ISO week
                 * number with century.
                 */
            do
                bp++;
            while (isdigit(*bp));
            continue;

        case 'V':	/* The ISO 8601:1988 week number as decimal */
            bp = conv_num(bp, &i, 0, 53);
            continue;

        case 'Y':	/* The year. */
            i = TM_YEAR_BASE;	/* just for data sanity... */
            bp = conv_num(bp, &i, 0, 9999);
            tm->tm_year = i - TM_YEAR_BASE;
            LEGAL_ALT(ALT_E);
            state |= S_YEAR;
            continue;

        case 'y':	/* The year within 100 years of the epoch. */
            /* LEGAL_ALT(ALT_E | ALT_O); */
            bp = conv_num(bp, &i, 0, 99);

            if (split_year)
                /* preserve century */
                i += (tm->tm_year / 100) * 100;
            else {
                split_year = 1;
                if (i <= 68)
                    i = i + 2000 - TM_YEAR_BASE;
            }
            tm->tm_year = i;
            state |= S_YEAR;
            continue;

        case 'Z':       // time zone name
        case 'z':       //
#ifdef _WIN32
            _tzset();
#else
            tzset();
#endif
            mandatory = c == 'z';
            if (mandatory)
                while (isspace(*bp))
                    bp++;

            zname = bp;
            switch (*bp++) {
            case 'G':
                if (*bp++ != 'M')
                    goto namedzone;
                /*FALLTHROUGH*/
            case 'U':
                if (*bp++ != 'T')
                    goto namedzone;
                else if (!delim(*bp) && *bp++ != 'C')
                    goto namedzone;
                /*FALLTHROUGH*/
            case 'Z':
                if (!delim(*bp))
                    goto namedzone;
                tm->tm_isdst = 0;
#ifdef TM_GMTOFF
                tm->TM_GMTOFF = 0;
#endif
#ifdef TM_ZONE
                tm->TM_ZONE = utc;
#endif
                continue;
            case '+':
                neg = 0;
                break;
            case '-':
                neg = 1;
                break;
            default:
namedzone:
                bp = zname;

                /* Nautical / Military style */
                if (delim(bp[1]) &&
                    ((*bp >= 'A' && *bp <= 'I') ||
                     (*bp >= 'L' && *bp <= 'Y'))) {
#ifdef TM_GMTOFF
                    /* Argh! No 'J'! */
                    if (*bp >= 'A' && *bp <= 'I')
                        tm->TM_GMTOFF =
                            (int)*bp - ('A' - 1);
                    else if (*bp >= 'L' && *bp <= 'M')
                        tm->TM_GMTOFF = (int)*bp - 'A';
                    else if (*bp >= 'N' && *bp <= 'Y')
                        tm->TM_GMTOFF = 'M' - (int)*bp;
                    tm->TM_GMTOFF *= SECSPERHOUR;
#endif
#ifdef TM_ZONE
                    tm->TM_ZONE = NULL; /* XXX */
#endif
                    bp++;
                    continue;
                }
                /* 'J' is local time */
                if (delim(bp[1]) && *bp == 'J') {
#ifdef TM_GMTOFF
                    tm->TM_GMTOFF = -timezone;
#endif
#ifdef TM_ZONE
                    tm->TM_ZONE = NULL; /* XXX */
#endif
                    bp++;
                    continue;
                }

                /*
                 * From our 3 letter hard-coded table
                 * XXX: Can be removed, handled by tzload()
                 */
                if (delim(bp[0]) || delim(bp[1]) ||
                    delim(bp[2]) || !delim(bp[3]))
                    goto loadzone;
                ep = find_string(bp, &i, nast, NULL, 4);
                if (ep != NULL) {
#ifdef TM_GMTOFF
                    tm->TM_GMTOFF = (-5 - i) * SECSPERHOUR;
#endif
#ifdef TM_ZONE
                    tm->TM_ZONE = __UNCONST(nast[i]);
#endif
                    bp = ep;
                    continue;
                }
                ep = find_string(bp, &i, nadt, NULL, 4);
                if (ep != NULL) {
                    tm->tm_isdst = 1;
#ifdef TM_GMTOFF
                    tm->TM_GMTOFF = (-4 - i) * SECSPERHOUR;
#endif
#ifdef TM_ZONE
                    tm->TM_ZONE = __UNCONST(nadt[i]);
#endif
                    bp = ep;
                    continue;
                }
                /*
                 * Our current timezone
                 */
                ep = find_string(bp, &i,
                             (const char * const *)tzname,
                              NULL, 2);
                if (ep != NULL) {
                    tm->tm_isdst = i;
#ifdef TM_GMTOFF
                    tm->TM_GMTOFF = -timezone;
#endif
#ifdef TM_ZONE
                    tm->TM_ZONE = tzname[i];
#endif
                    bp = ep;
                    continue;
                }
loadzone:
                /*
                 * The hard way, load the zone!
                 */
                if (fromzone(&bp, tm, mandatory))
                    continue;
                goto out;
            }
            offs = 0;
            for (i = 0; i < 4; ) {
                if (isdigit(*bp)) {
                    offs = offs * 10 + (*bp++ - '0');
                    i++;
                    continue;
                }
                if (i == 2 && *bp == ':') {
                    bp++;
                    continue;
                }
                break;
            }
            if (isdigit(*bp))
                goto out;
            switch (i) {
            case 2:
                offs *= SECSPERHOUR;
                break;
            case 4:
                i = offs % 100;
                offs /= 100;
                if (i >= SECSPERMIN)
                    goto out;
                /* Convert minutes into decimal */
                offs = offs * SECSPERHOUR + i * SECSPERMIN;
                break;
            default:
out:
                if (mandatory)
                    return NULL;
                bp = zname;
                continue;
            }
            /* ISO 8601 & RFC 3339 limit to 23:59 max */
            if (offs >= (HOURSPERDAY * SECSPERHOUR))
                goto out;
            if (neg)
                offs = -offs;
            tm->tm_isdst = 0;	/* XXX */
#ifdef TM_GMTOFF
            tm->TM_GMTOFF = offs;
#endif
#ifdef TM_ZONE
            tm->TM_ZONE = NULL;	/* XXX */
#endif
            continue;

        /*
         * Miscellaneous conversions.
         */
        case 'n':	/* Any kind of white-space. */
        case 't':
            while (isspace(*bp))
                bp++;
            LEGAL_ALT(0);
            continue;


        default:	/* Unknown/unsupported conversion. */
            return NULL;
        }
    }

    if (!HAVE_YDAY(state) && HAVE_YEAR(state)) {
        if (HAVE_MON(state) && HAVE_MDAY(state)) {
            /* calculate day of year (ordinal date) */
            tm->tm_yday =  start_of_month[isleap_sum(tm->tm_year,
                TM_YEAR_BASE)][tm->tm_mon] + (tm->tm_mday - 1);
            state |= S_YDAY;
        } 
        else if (day_offset != -1) {
            /*
             * Set the date to the first Sunday (or Monday)
             * of the specified week of the year.
             */
            if (!HAVE_WDAY(state)) {
                tm->tm_wday = day_offset;
                state |= S_WDAY;
            }
            tm->tm_yday = (7 -
                first_wday_of(tm->tm_year + TM_YEAR_BASE) +
                day_offset) % 7 + (week_offset - 1) * 7 +
                tm->tm_wday  - day_offset;
            state |= S_YDAY;
        }
    }

    if (HAVE_YDAY(state) && HAVE_YEAR(state)) {
        int isleap;

        if (!HAVE_MON(state)) {
            /* calculate month of day of year */
            i = 0;
            isleap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
            while (tm->tm_yday >= start_of_month[isleap][i])
                i++;
            if (i > 12) {
                i = 1;
                tm->tm_yday -= start_of_month[isleap][12];
                tm->tm_year++;
            }
            tm->tm_mon = i - 1;
            state |= S_MON;
        }

        if (!HAVE_MDAY(state)) {
            /* calculate day of month */
            isleap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
            tm->tm_mday = tm->tm_yday -
                start_of_month[isleap][tm->tm_mon] + 1;
            state |= S_MDAY;
        }

        if (!HAVE_WDAY(state)) {
            /* calculate day of week */
            i = 0;
            week_offset = first_wday_of(tm->tm_year);
            while (i++ <= tm->tm_yday) {
                if (week_offset++ >= 6)
                    week_offset = 0;
            }
            tm->tm_wday = week_offset;
            state |= S_WDAY;
        }
    }

    return (char*)bp;
}


static const unsigned char *conv_num(const unsigned char *buf, int *dest, unsigned int llim, unsigned int ulim) {
    if (!buf || !dest) {
        fmt_fprintf(stderr, "Error: NULL pointer passed to conv_num.\n");
        return NULL;
    }
    unsigned int result = 0;
    unsigned char ch;

    /* The limit also determines the number of valid digits. */
    unsigned int rulim = ulim;

    ch = *buf;
    if (ch < '0' || ch > '9') {
        fmt_fprintf(stderr, "Error: Non-numeric character found in conv_num.\n");
        return NULL;
    }

    do {
        result *= 10;
        result += ch - '0';
        rulim /= 10;
        ch = *++buf;
    } while ((result <= ulim) && rulim && ch >= '0' && ch <= '9');

    if (result < llim || result > ulim) {
        return NULL;
    }

    *dest = result;
    return buf;
}

static const unsigned char *find_string(const unsigned char *bp, int *tgt, const char * const *n1,
        const char * const *n2, int c) {

    if (!bp || !tgt || !n1) {
        fmt_fprintf(stderr, "Error: NULL pointer passed to find_string.\n");
        return NULL;
    }
    int i;
    size_t len;

    /* check full name - then abbreviated ones */
    for (; n1 != NULL; n1 = n2, n2 = NULL) {
        for (i = 0; i < c; i++, n1++) {
            len = strlen(*n1);
            if (strncasecmp(*n1, (const char *)bp, len) == 0) {
                *tgt = i;
                return bp + len;
            }
        }
    }

    /* Nothing matched */
    return NULL;
}
#endif

static bool is_persian_leap_year(int year) { // winner (Musa Akrami method) ! ;D
    if (year <= 0) {
        fmt_fprintf(stderr, "Error: Invalid year passed to is_persian_leap_year. Year: %d\n", year);
        return false;
    }

	const double QUARTER_DAY = 0.24219858156;
	double testLeap = (year + 2346) * QUARTER_DAY;
	testLeap -= (int)(testLeap);

	if (testLeap < QUARTER_DAY) {
        return true;
    }
	else return false;
}

static int persian_days_in_month(int year, int month) {
    if (year <= 0 || month <= 0 || month > 12) {
        fmt_fprintf(stderr, "Error: Invalid year or month passed to persian_days_in_month. Year: %d, Month: %d\n", year, month);
        return -1; // Indicate an error
    }
    if (month <= 6) {
        return 31; // The first six months have 31 days
    } 
    else if (month <= 11) {
        return 30; // The next five months have 30 days
    } 
    else {
        // The last month has 29 or 30 days depending on leap year
        return is_persian_leap_year(year) ? 30 : 29;
    }
}

static bool date_is_valid_ymd(int y, int m, int d, CalendarType type) {
    if (y == 0 || m <= 0 || m > 12 || d <= 0) {
        fmt_fprintf(stderr, "Error: Invalid year, month, or day in date_is_valid_ymd. Year: %d, Month: %d, Day: %d\n", y, m, d);
        return false;
    }

    if (type == Gregorian) {
        // Existing Gregorian logic...
        int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) {
            days_in_month[2] = 29; // February in a leap year
        }
        return d <= days_in_month[m];
    } 
    else if (type == Persian) {
        return d <= persian_days_in_month(y, m);
    }
    return false;
}

static int days_in_month(int year, int month, CalendarType type) {
    if (month < 1 || month > 12) {
        fmt_fprintf(stderr, "Error: Invalid month in days_in_month. Month: %d\n", month);
        return 0; // Return 0 for invalid month
    }
    if (type == Gregorian) {
        // Existing Gregorian logic...
        int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)) {
            return 29;
        } 
        else {
            return days_in_month[month];
        }
    } 
    else if (type == Persian) {
        return persian_days_in_month(year, month);
    }

    return 0; // Default case for unsupported calendar types
}

bool date_is_valid(const Date* date) {
    if (!date) {
        fmt_fprintf(stderr, "Error: Date pointer is NULL in date_is_valid.\n");
        return false;
    }
    return date_is_valid_ymd(date->year, date->month, date->day, date->calendarType);
}

Date* date_create(CalendarType type) {
    Date* date = (Date*)malloc(sizeof(Date));
    if (!date) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in date_create.\n");
        exit(-1);
    }

    // Initialize with invalid values to signify a null (invalid) date
    date->year = -1;
    date->month = -1;
    date->day = -1;
    date->calendarType = type;

    return date;
}

Date* date_create_ymd(int y, int m, int d, CalendarType type) {
    Date* date = (Date*)malloc(sizeof(Date));
    if (!date) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in date_create_ymd.\n");
        exit(-1);
    }

    if (!date_is_valid_ymd(y, m, d, type)) {
        fmt_fprintf(stderr, "Error: Invalid date parameters in date_create_ymd. Year: %d, Month: %d, Day: %d\n", y, m, d);
        free(date); // Release the allocated memory
        exit(-1);
    }

    date->year = y;
    date->month = m;
    date->day = d;
    date->calendarType = type;
    return date;
}

bool date_is_null(const Date* date) {
    if (!date) {
        fmt_fprintf(stderr, "Warning: Date pointer is NULL in date_is_null.\n");
        return true;
    }
    return false;
}

Date* date_add_days(const Date* orig_date, int ndays) {
    if (!orig_date) {
        fmt_fprintf(stderr, "Error: Original date is NULL in date_add_days.\n");
        return NULL;
    }
    if (!date_is_valid_ymd(orig_date->year, orig_date->month, orig_date->day, orig_date->calendarType)) {
        fmt_fprintf(stderr, "Error: Invalid original date in date_add_days. Year: %d, Month: %d, Day: %d\n", orig_date->year, orig_date->month, orig_date->day);
        return NULL;
    }

    Date* new_date = (Date*)malloc(sizeof(Date));
    if (!new_date) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for new date in date_add_days.\n");
        return NULL;
    }

    *new_date = *orig_date;

    while (ndays != 0) {
        if (ndays > 0) {
            // Adding days
            int days_this_month = days_in_month(new_date->year, new_date->month, new_date->calendarType);
            if (new_date->day + ndays > days_this_month) {
                ndays -= (days_this_month - new_date->day + 1);
                new_date->day = 1;
                if (++new_date->month > (new_date->calendarType == Persian && new_date->year % 33 == 1 ? 13 : 12)) {
                    new_date->month = 1;
                    new_date->year++;
                }
            } 
            else {
                new_date->day += ndays;
                ndays = 0;
            }
        } 
        else {
            // Subtracting days
            if (new_date->day + ndays < 1) {
                ndays += new_date->day;
                if (--new_date->month < 1) {
                    new_date->month = (new_date->calendarType == Persian && (new_date->year - 1) % 33 == 1 ? 13 : 12);
                    new_date->year--;
                }
                new_date->day = days_in_month(new_date->year, new_date->month, new_date->calendarType);
            } 
            else {
                new_date->day += ndays;
                ndays = 0;
            }
        }
    }
    return new_date;
}

Date* date_add_months(const Date* orig_date, int nmonths) {
    if (orig_date == NULL || !date_is_valid_ymd(orig_date->year, orig_date->month, orig_date->day, orig_date->calendarType)) {
        fmt_fprintf(stderr, "Error: date is null or not valid in date_add_months.\n");
        exit(-1);
    }

    Date* new_date = (Date*)malloc(sizeof(Date));
    if (new_date == NULL) {
        fmt_fprintf(stderr, "Error: Cannot allocate memory for new date in date_add_months.\n");
        exit(-1);
    }

    *new_date = *orig_date;
    int years_to_add = nmonths / 12;
    nmonths %= 12;

    new_date->year += years_to_add;
    new_date->month += nmonths;

    // Adjust the number of months based on calendar type
    int totalMonthsInYear = (orig_date->calendarType == Persian && new_date->year % 33 == 1) ? 13 : 12;
    if (new_date->month > totalMonthsInYear) {
        new_date->year++;
        new_date->month -= totalMonthsInYear;
    }

    int days_this_month = days_in_month(new_date->year, new_date->month, orig_date->calendarType);
    if (new_date->day > days_this_month) {
        new_date->day = days_this_month;
    }
    return new_date;
}

Date* date_add_years(const Date* orig_date, int nyears) {
    if (orig_date == NULL || !date_is_valid_ymd(orig_date->year, orig_date->month, orig_date->day, orig_date->calendarType)) {
        fmt_fprintf(stderr, "Error: date is null or not valid in date_add_years.\n");
        exit(-1);
    }

    Date* new_date = (Date*)malloc(sizeof(Date));
    if (new_date == NULL) {
        fmt_fprintf(stderr, "Error: Cannot allocate memory for new date in date_add_years.\n");
        exit(-1);
    }

    *new_date = *orig_date;
    new_date->year += nyears;

    // Adjust for leap years
    if (orig_date->calendarType == Gregorian) {
        if (new_date->month == 2 && new_date->day == 29 && !date_is_leap_year_y(new_date->year, Gregorian)) {
            new_date->day = 28; // Adjust for non-leap year
        }
    } 
    else if (orig_date->calendarType == Persian) {
        // Check if the original date is the extra day in a leap year
        if (new_date->month == 12 && new_date->day == 30 && !date_is_leap_year_y(new_date->year, Persian)) {
            new_date->day = 29; // Adjust for non-leap year
        }
    }

    return new_date;
}

void date_get_date(const Date* date, int *year, int *month, int *day) {
     if (!date) {
        fmt_fprintf(stderr, "Error: Passed 'date' is NULL in date_get_date.\n");
        return;
    }
    if (!year || !month || !day) {
        fmt_fprintf(stderr, "Error: One or more output pointers (year, month, day) are NULL in date_get_date.\n");
        return;
    }

    *year = date->year;
    *month = date->month;
    *day = date->day;
}

int date_day(const Date* date) {
    if (date == NULL || !date_is_valid(date)) {
        // Handle the error, as the date is NULL
        fmt_fprintf(stderr, "Error: Date is null in date_day or date is not valid.\n");
        return -1; // Return an invalid day
    }
    return date->day;
}

int date_month(const Date* date) {
    if (date == NULL || !date_is_valid(date)) {
        // Handle the error, as the date is NULL
        fmt_fprintf(stderr, "Error: Date is null in date_month or date is not valid.\n");
        return -1; // Return an invalid month
    }
    return date->month;
}

int date_year(const Date* date) {
    if (date == NULL) {
        // Handle the error, as the date is NULL
        fmt_fprintf(stderr, "Error: Date is null in date_year or date is not valid.\n");
        return -1; // Return an invalid year
    }
    return date->year;
}

static long gregorian_to_jdn(int year, int month, int day) {
    if (month < 3) {
        year--;
        month += 12;
    }
    return day + (153 * (month - 3) + 2) / 5 + 365 * year + year / 4 - year / 100 + year / 400 - 32045;
}

// Helper function to convert Persian date to Julian Day Number
static long persian_to_jdn(int year, int month, int day) {
    int epbase, epyear;

    epbase = year - ((year >= 0) ? 474 : 473);
    epyear = 474 + (epbase % 2820);

    return day +
           ((month <= 7) ? ((month - 1) * 31) : (((month - 1) * 30) + 6)) +
           ((epyear * 682) - 110) / 2816 +
           (epyear - 1) * 365 +
           (epbase / 2820) * 1029983 +
           (1948320 - 1);
}

// This function returns the day of the week for the given date, where Monday is 1 and Sunday is 7.
int date_day_of_week(const Date* date) {
    if (date == NULL) {
        fmt_fprintf(stderr, "Error: Date is null in date_day_of_week.\n");
        return -1; // Indicate an error
    }

    long jdn;
    if (date->calendarType == Gregorian) {
        jdn = gregorian_to_jdn(date->year, date->month, date->day);
    } 
    else if (date->calendarType == Persian) {
        jdn = persian_to_jdn(date->year, date->month, date->day);
    } 
    else {
        fmt_fprintf(stderr, "Error: Unsupported calendar type in date_day_of_week.\n");
        return -1; // Indicate an error for unsupported calendar types
    }
    // Calculate the day of the week (Monday = 1, ..., Sunday = 7)
    return (jdn + 1) % 7 + 1;
}

// This function calculates the day of the year, from 1 to 365 or 366 in a leap year.
int date_day_of_year(const Date* date) {
    if (date == NULL) {
        fmt_fprintf(stderr, "Error: Date is null in date_day_of_year.\n");
        return -1; // Indicate an error
    }

    int dayOfYear;
    if (date->calendarType == Gregorian) {
        static const int daysBeforeMonthGregorian[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
        dayOfYear = daysBeforeMonthGregorian[date->month - 1] + date->day;

        // Add one day if it's a leap year and after February
        if (date->month > 2 && ((date->year % 4 == 0 && date->year % 100 != 0) || (date->year % 400 == 0))) {
            dayOfYear++;
        }
    } 
    else if (date->calendarType == Persian) {
        static const int daysBeforeMonthPersian[] = {0, 31, 62, 93, 124, 155, 186, 216, 246, 276, 306, 336, 365};
        dayOfYear = daysBeforeMonthPersian[date->month - 1] + date->day;
        // Adjust for leap year
        if (date->month > 11 || (date->month == 11 && date->day == 30)) {
            if (is_persian_leap_year(date->year)) {
                dayOfYear++;
            }
        }
    } 
    else {
        fmt_fprintf(stderr, "Error: Unsupported calendar type in date_day_of_year.\n");
        return -1; // Indicate an error for unsupported calendar types
    }

    return dayOfYear;
}
// This function returns the number of days in the given month of the specified year.
int date_days_in_month(const Date* date) {
    if (!date) {
        fmt_fprintf(stderr, "Error: Date pointer is NULL in date_days_in_month.\n");
        return -1; // Indicate an error
    }

    int days;
    if (date->calendarType == Gregorian) {
        static const int daysInMonthGregorian[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        days = daysInMonthGregorian[date->month - 1];
        // Check for leap year in February
        if (date->month == 2 && ((date->year % 4 == 0 && date->year % 100 != 0) || (date->year % 400 == 0))) {
            days++;
        }
    } 
    else if (date->calendarType == Persian) {
        // Persian calendar months: 6 months of 31 days, 5 months of 30 days, and 29 or 30 days in the last month
        if (date->month <= 6) {
            days = 31;
        } 
        else if (date->month <= 11) {
            days = 30;
        } 
        else { // 12th month
            days = is_persian_leap_year(date->year) ? 30 : 29;
        }
    } 
    else {
        fmt_fprintf(stderr, "Error: Unsupported calendar type in date_days_in_month.\n");
        return -1; // Indicate an error for unsupported calendar types
    }

    return days;
}

// This function returns the total number of days in the year of the given date.
int date_days_in_year(const Date* date) {
    if (!date) {
        fmt_fprintf(stderr, "Error: Date pointer is NULL in date_days_in_year.\n");
        return -1; // Indicate an error
    }

    if (date->calendarType == Gregorian) {
        if ((date->year % 4 == 0 && date->year % 100 != 0) || (date->year % 400 == 0)) {
            return 366; // Gregorian leap year
        } 
        else {
            return 365; // Gregorian non-leap year
        }
    } 
    else if (date->calendarType == Persian) {
        if (is_persian_leap_year(date->year)) {
            return 366; // Persian leap year
        } 
        else {
            return 365; // Persian non-leap year
        }
    } 
    else {
        fmt_fprintf(stderr, "Error: Unsupported calendar type in date_days_in_year.\n");
        return -1; // Indicate an error for unsupported calendar types
    }
}

// This function calculates the ISO 8601 week number for the given date.
int date_week_number(const Date* date, int* yearNumber) {
    if (!date) {
        fmt_fprintf(stderr, "Error: Date is NULL in date_week_number.\n");
        return -1;
    }

    int dayOfYear = date_day_of_year(date);
    int wday = date_day_of_week(date);

    // Determine the week number based on the calendar type
    int week;
    if (date->calendarType == Gregorian) {
        // ISO week number calculation for Gregorian calendar
        week = ((dayOfYear - wday + 10) / 7);

        if (yearNumber != NULL) {
            if (week < 1) {
                *yearNumber = date->year - 1;
            } 
            else if (week == 53 && (date_day_of_year(&(Date){date->year, 12, 31, Gregorian}) - wday) < 28) {
                *yearNumber = date->year + 1;
            } 
            else {
                *yearNumber = date->year;
            }
        }
    } 
    else if (date->calendarType == Persian) {
        // Simplified week number calculation for Persian calendar
        week = (dayOfYear + 6) / 7;

        if (yearNumber != NULL) {
            if (week < 1) {
                *yearNumber = date->year - 1;
            } 
            else if (week >= 53 && dayOfYear < 7) {
                *yearNumber = date->year + 1;
            } 
            else {
                *yearNumber = date->year;
            }
        }
    } 
    else {
        fmt_fprintf(stderr, "Error: Unsupported calendar type in date_week_number.\n");
        return -1; // Indicate an error for unsupported calendar types
    }

    return week;
}

// This function calculates the number of days between two dates.
int date_days_to(const Date* from, const Date* to) {
    if (!from || !to) {
        fmt_fprintf(stderr, "Error: One or both date pointers are NULL in date_days_to.\n");
        return -1;
    }
    if (from->calendarType != to->calendarType) {
        fmt_fprintf(stderr, "Error: Calendar types of dates do not match in date_days_to.\n");
        return -1;
    }
    if (!date_is_valid(from)) {
        fmt_fprintf(stderr, "Error: The 'from' date is invalid in date_days_to.\n");
        return -1;
    }
    if (!date_is_valid(to)) {
        fmt_fprintf(stderr, "Error: The 'to' date is invalid in date_days_to.\n");
        return -1;
    }

    // Convert both dates to Julian Day Number and find the difference
    long from_jd = date_to_julian_day(from);
    long to_jd = date_to_julian_day(to);

    return (int)(to_jd - from_jd);
}

bool date_is_equal(const Date* lhs, const Date* rhs) {
    if (lhs == NULL || rhs == NULL || !date_is_valid(lhs) || !date_is_valid(rhs) || (lhs->calendarType != rhs->calendarType)) {
        fmt_fprintf(stderr, "Error: One or both dates are null in date_is_equals.\n");
        return false;  // Consider unequal if either date is NULL
    }
    return (lhs->year == rhs->year) && (lhs->month == rhs->month) && (lhs->day == rhs->day);
}

bool date_is_less_than(const Date* lhs, const Date* rhs) {
    if (lhs == NULL || rhs == NULL || !date_is_valid(lhs) || !date_is_valid(rhs) || (lhs->calendarType != rhs->calendarType)) {
        fmt_fprintf(stderr, "Error: One or both dates are null or invalid in date_is_less_than.\n");
        return false;  // Cannot determine if invalid or NULL
    }
    if (lhs->year != rhs->year) {
        return lhs->year < rhs->year;
    }
    if (lhs->month != rhs->month) {
        return lhs->month < rhs->month;
    }
    return lhs->day < rhs->day;
}

bool date_is_less_than_or_equal(const Date* lhs, const Date* rhs) {
    if (lhs == NULL || rhs == NULL || !date_is_valid(lhs) || !date_is_valid(rhs) || (lhs->calendarType != rhs->calendarType)) {
        fmt_fprintf(stderr, "Error: One or both dates are null or invalid in date_is_less_than_or_equal.\n");
        return false;  // Cannot determine if invalid or NULL
    }
    return date_is_less_than(lhs, rhs) || date_is_equal(lhs, rhs);
}

bool date_is_greater_than(const Date* lhs, const Date* rhs) {
    if (lhs == NULL || rhs == NULL || !date_is_valid(lhs) || !date_is_valid(rhs) || (lhs->calendarType != rhs->calendarType)) {
        fmt_fprintf(stderr, "Error: One or both dates are null or invalid in date_is_greater_than.\n");
        return false;  // Cannot determine if invalid or NULL
    }
    if (lhs->year != rhs->year) {
        return lhs->year > rhs->year;
    }
    if (lhs->month != rhs->month) {
        return lhs->month > rhs->month;
    }
    return lhs->day > rhs->day;
}

bool date_is_greater_than_or_equal(const Date* lhs, const Date* rhs) {
    if (lhs == NULL || rhs == NULL || !date_is_valid(lhs) || !date_is_valid(rhs) || (lhs->calendarType != rhs->calendarType)) {
        fmt_fprintf(stderr, "Error: One or both dates are null or invalid in date_is_greater_than_or_equal.\n");
        return false;  // Cannot determine if invalid or NULL
    }
    return date_is_greater_than(lhs, rhs) || date_is_equal(lhs, rhs);
}

bool date_is_not_equals(const Date* lhs, const Date* rhs) {
    return !date_is_equal(lhs, rhs);
}

bool date_is_leap_year_y(int year, CalendarType type) {
    if (type != Gregorian && type != Persian) {
        fmt_fprintf(stderr, "Error: CalendarType is not valid in date_is_leap_year_y.\n");
        return false;
    }
    if (type == Gregorian) {
        // Gregorian leap year calculation
        if (year % 4 == 0) {
            if (year % 100 == 0) {
                return year % 400 == 0;
            }
            return true;
        }
        return false;
    } 
    else if (type == Persian) {
        // Persian leap year calculation
        return is_persian_leap_year(year);
    }
    return false; // Default case
}

bool date_is_leap_year(const Date* date) {
    if (date == NULL ) {
        fmt_fprintf(stderr, "Error: Date is null in date_is_leap_year");
        return false;  // Cannot determine if the date is NULL
    }
    return date_is_leap_year_y(date->year, date->calendarType);
}

bool date_set_date(Date* date, int year, int month, int day, CalendarType type) {
    if (date == NULL) {
        fmt_fprintf(stderr, "Error: Date is null in date_set_date");
        return false;
    }
    if (year <= 0 || month <= 0 || month > 12 || day <= 0) {
        fmt_fprintf(stderr, "Error: Year, month, or day are not valid");
        return false;
    }

    // Check validity based on calendar type
    if ((type == Gregorian && day > days_in_month(year, month, Gregorian)) ||
        (type == Persian && day > days_in_month(year, month, Persian))) {
        fmt_fprintf(stderr, "Error: Day is not valid for the given month and year");
        return false;
    }

    date->year = year;
    date->month = month;
    date->day = day;
    date->calendarType = type;

    return true;
}

Date* date_current_date(CalendarType type) {
    // Get the current time
    time_t now = time(NULL);
    struct tm *current = localtime(&now);

    // Allocate memory for a new Date
    Date* currentDate = (Date*)malloc(sizeof(Date));
    if (currentDate == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in date_current_date.\n");
        return NULL;
    }
    if (type == Gregorian) {
        currentDate->year = current->tm_year + 1900; // tm_year is years since 1900
        currentDate->month = current->tm_mon + 1;    // tm_mon is months since January (0-11)
        currentDate->day = current->tm_mday;
        currentDate->calendarType = Gregorian;
    } 
    else if (type == Persian) {
        // Convert Gregorian date to Persian date
        Date gregorianDate = {current->tm_year + 1900, current->tm_mon + 1, current->tm_mday, Gregorian};
        Date* persianDate = date_gregorian_to_solar(&gregorianDate);
        if (persianDate) {
            *currentDate = *persianDate;
            free(persianDate);
            currentDate->calendarType = Persian;
        } 
        else {
            fmt_fprintf(stderr, "Error: Failed to get current Persian Date in date_current_date.\n");
            free(currentDate);
            return NULL;
        }
    } 
    else {
        fmt_fprintf(stderr, "Error: Unsupported calendar type in date_current_date.\n");
        free(currentDate);
        return NULL;
    }

    return currentDate;
}

Date* date_from_string(const char* string, const char* format, CalendarType type) {
    if (string == NULL || format == NULL) {
        fmt_fprintf(stderr, "Error: Invalid argument passed to date_from_string");
        return NULL;
    }

    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));

    // Allocate memory for a new Date
    Date* date = (Date*)malloc(sizeof(Date));
    if (date == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in date_from_string.\n");
        return NULL;
    }

    date->calendarType = type;

    if (type == Gregorian) {
        #if defined(_WIN32) || defined(_WIN64)
        if (win_strptime(string, format, &tm) == NULL) {
            fmt_fprintf(stderr, "Error: failed to parse string in date_from_string.\n");
            free(date);
            return NULL; // Failed to parse string
        }
        #else 
        if (strptime(string, format, &tm) == NULL) {
            fmt_fprintf(stderr, "Error: failed to parse string in date_from_string.\n");
            free(date);
            return NULL; // Failed to parse string
        }
        #endif

        date->year = tm.tm_year + 1900; // tm_year is years since 1900
        date->month = tm.tm_mon + 1;    // tm_mon is months since January (0-11)
        date->day = tm.tm_mday;
    } 
    else if (type == Persian) {
        // Basic parsing for a Persian date in "YYYY-MM-DD" format
        if (sscanf(string, "%d-%d-%d", &date->year, &date->month, &date->day) != 3) {
            fmt_fprintf(stderr, "Error: failed to parse string in date_from_string.\n");
            free(date);
            return NULL; // Failed to parse string
        }

        // Additional validation can be added here for Persian dates
    } 
    else {
        fmt_fprintf(stderr, "Error: Unsupported calendar type in date_from_string.\n");
        free(date);
        return NULL;
    }

    return date;
}

char* date_to_string(const Date* date, const char* format) {
    if (!date || !format) {
        fmt_fprintf(stderr, "Error: Invalid argument passed to date_to_string. Date or format is NULL.\n");
        return NULL;
    }

    // Allocate buffer for the formatted date string
    char* date_str = (char*)malloc(80 * sizeof(char));  // Adjust size as necessary
    if (!date_str) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for date string in date_to_string.\n");
        return NULL;
    }

    if (date->calendarType == Gregorian) {
        // Prepare a struct tm from the Date struct for Gregorian calendar
        struct tm tm_date = {
            .tm_year = date->year - 1900,  // tm_year expects years since 1900
            .tm_mon = date->month - 1,     // tm_mon expects months from 0 to 11
            .tm_mday = date->day,
            .tm_isdst = -1  // Let the function determine whether DST is in effect
        };
        // Format the date using strftime
        if (strftime(date_str, 80, format, &tm_date) == 0) {
            fmt_fprintf(stderr, "Error: Failed to format date in date_to_string.\n");
            free(date_str);
            return NULL;
        }
    } 
    else if (date->calendarType == Persian) {
        int result = snprintf(date_str, 80, "%04d-%02d-%02d", date->year, date->month, date->day);
        if (result < 0 || result >= 80) {
            fmt_fprintf(stderr, "Error: Failed to format Persian date in date_to_string.\n");
            free(date_str);
            return NULL;
        }
    } 
    else {
        fmt_fprintf(stderr, "Error: Unsupported calendar type in date_to_string.\n");
        free(date_str);
        return NULL;
    }

    return date_str;
}

long date_to_julian_day(const Date* date) {
    if (!date) {
        fmt_fprintf(stderr, "Error: Date is NULL in date_to_julian_day.\n");
        return -1;
    }
    if (!date_is_valid(date)) {
        fmt_fprintf(stderr, "Error: Invalid date in date_to_julian_day.\n");
        return -1;
    }

    int a = (14 - date->month) / 12;
    int y = date->year + 4800 - a;
    int m = date->month + 12 * a - 3;

    long jd = date->day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;

    return jd;
}

Date* date_from_julian_day(long jd) {
    if (jd < 0) {
        fmt_fprintf(stderr, "Error: Invalid Julian Day Number in date_from_julian_day.\n");
        return NULL;
    }

    long j = jd + 32044;
    long g = j / 146097;
    long dg = j % 146097;
    long c = (dg / 36524 + 1) * 3 / 4;
    long dc = dg - c * 36524;
    long b = dc / 1461;
    long db = dc % 1461;
    long a = (db / 365 + 1) * 3 / 4;
    long da = db - a * 365;
    long y = g * 400 + c * 100 + b * 4 + a;
    long m = (da * 5 + 308) / 153 - 2;
    long d = da - (m + 4) * 153 / 5 + 122;

    Date* date = (Date*)malloc(sizeof(Date));
    if (!date) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for date in date_from_julian_day.\n");
        return NULL;
    }

    date->year = y - 4800 + (m + 2) / 12;
    date->month = (m + 2) % 12 + 1;
    date->day = d + 1;

    return date;
}

Date* date_gregorian_to_solar(const Date* gregorian_date) {
    if (!gregorian_date) {
        fmt_fprintf(stderr, "Error: The provided Gregorian date is NULL in date_gregorian_to_solar.\n");
        return NULL;
    }
    long gy = gregorian_date->year;
    long gm = gregorian_date->month;
    long gd = gregorian_date->day;
    long jalali_date[3];
    long days;
    
    // Calculate number of days
    long gy2 = (gm > 2) ? (gy + 1) : gy;
    long g_d_m[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    days = 355666 + (365 * gy) + ((gy2 + 3) / 4) - ((gy2 + 99) / 100) + ((gy2 + 399) / 400) + gd + g_d_m[gm - 1];
    
    // Convert to Jalali
    long jy = -1595 + (33 * (days / 12053));
    days %= 12053;
    jy += 4 * (days / 1461);
    days %= 1461;

    if (days > 365) {
        jy += (days - 1) / 365;
        days = (days - 1) % 365;
    }

    jalali_date[0] = jy;
    if (days < 186) {
        jalali_date[1] = 1 + (days / 31);
        jalali_date[2] = 1 + (days % 31);
    } 
    else {
        jalali_date[1] = 7 + ((days - 186) / 30);
        jalali_date[2] = 1 + ((days - 186) % 30);
    }

    // Create new Date for Jalali
    Date* solar_date = (Date*)malloc(sizeof(Date));
    if (!solar_date) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in date_gregorian_to_solar.\n");
        return NULL;
    }

    solar_date->year = jalali_date[0];
    solar_date->month = jalali_date[1];
    solar_date->day = jalali_date[2];
    solar_date->calendarType = Persian;

    return solar_date;
}

Date* date_solar_to_gregorian(const Date* solar_date) {
    if (!solar_date) {
        fmt_fprintf(stderr, "Error: The provided Persian date is NULL in date_solar_to_gregorian.\n");
        return NULL;
    }

    long jy = solar_date->year;
    long jm = solar_date->month;
    long jd = solar_date->day;
    
    if (solar_date->month == 12 && solar_date->day > 29 && !is_persian_leap_year(solar_date->year)) {
        fmt_fprintf(stderr, "Error: the date is not valid because year is not leap year");
        exit(-1);
    }
    if (jm == 12 && jd == 30 && date_is_leap_year(solar_date)) {
        jd = 29; // Adjust the day for calculation
    }
    // Adjust for Persian calendar specifics
    jy += 1595;
    long days = -355668 + (365 * jy) + ((jy / 33) * 8) + (((jy % 33) + 3) / 4);

    // Adjust for months
    if (jm < 7) {
        days += (jm - 1) * 31;
    } 
    else {
        days += ((jm - 7) * 30) + 186;
    }

    // Adjust for leap years and days in month
    if (jm == 12 && is_persian_leap_year(solar_date->year) && jd > 29) {
        jd = 29; // Adjust the day in a leap year
    }
    days += jd;

    // Convert to Gregorian
    long gy = 400 * (days / 146097);
    days %= 146097;
    if (days > 36524) {
        gy += 100 * (--days / 36524);
        days %= 36524;
        if (days >= 365) days++;
    }

    gy += 4 * (days / 1461);
    days %= 1461;
    if (days > 365) {
        gy += (days - 1) / 365;
        days = (days - 1) % 365;
    }

    long gd = days + 1;
    long gm;
    long sal_a[13] = {0, 31, (gy % 4 == 0 && (gy % 100 != 0 || gy % 400 == 0)) ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    for (gm = 0; gm < 13 && gd > sal_a[gm]; gm++) {
        gd -= sal_a[gm];
    }

    Date* gregorian_new_date = (Date*)malloc(sizeof(Date));
    if (!gregorian_new_date) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in date_solar_to_gregorian.\n");
        return NULL;
    }

    gregorian_new_date->year = gy;
    gregorian_new_date->month = gm;
    gregorian_new_date->day = gd;

    if (jm == 12 && is_persian_leap_year(solar_date->year) && solar_date->day == 30) {
        gregorian_new_date->day += 1;
    }
    gregorian_new_date->calendarType = Gregorian;
    return gregorian_new_date;
}

void date_deallocate(Date* date) {
    if (!date) {
        fmt_fprintf(stderr, "Warning: Attempted to deallocate a NULL date in date_deallocate.\n");
        return;
    }
    free(date);
}
