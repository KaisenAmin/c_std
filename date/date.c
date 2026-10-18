/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Date
*/

#if defined(__linux__)
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <unistd.h>
#endif 

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "date.h"
#include "../time/std_time.h"


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

char* win_strptime(const char *buf, const char *fmt, struct tm *tm) {
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

                /* Overflow is tested before each step: the old guard computed
                   sse * 10 itself, which is signed overflow (UB) for long
                   digit runs. Too many digits is a parse failure. */
                bool overflow = false;
                do {
                    int digit = *bp - '0';
                    if (sse > ((time_t)TIME_MAX - digit) / 10) {
                        overflow = true;
                        break;
                    }
                    sse = sse * 10 + digit;
                    bp++;
                    rulim /= 10;
                } while (rulim && *bp >= '0' && *bp <= '9');

                if (overflow || sse < 0 || (uint64_t)sse > TIME_MAX) {
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
            /* Test before advancing: the old do/while stepped over the
               terminator of an exhausted input and read past it. */
            if (!isdigit(*bp))
                return NULL;
            while (isdigit(*bp))
                bp++;
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
                /* No zone text left: bp[1] below would be past the terminator. */
                if (*bp == '\0')
                    goto out;

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

    /* tm_mon indexes the 13-entry start_of_month rows below. */
    if (HAVE_MON(state) && (tm->tm_mon < 0 || tm->tm_mon > 11))
        return NULL;

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
            /* calculate month of day of year. The ordinal comes from %j or
               the week arithmetic and can be negative or past the year's
               end; the old scan then indexed start_of_month out of bounds. */
            if (tm->tm_yday < 0)
                return NULL;
            isleap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
            while (tm->tm_yday >= start_of_month[isleap][12]) {  /* spills into the next year */
                tm->tm_yday -= start_of_month[isleap][12];
                tm->tm_year++;
                if (tm->tm_year > 9999 - TM_YEAR_BASE)
                    return NULL;
                isleap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
            }
            i = 0;
            while (tm->tm_yday >= start_of_month[isleap][i + 1])
                i++;
            tm->tm_mon = i;
            state |= S_MON;
        }

        if (!HAVE_MDAY(state)) {
            /* calculate day of month (tm_mon indexes start_of_month[][13]) */
            if (tm->tm_mon < 0 || tm->tm_mon > 11)
                return NULL;
            isleap = isleap_sum(tm->tm_year, TM_YEAR_BASE);
            tm->tm_mday = tm->tm_yday -
                start_of_month[isleap][tm->tm_mon] + 1;
            state |= S_MDAY;
        }

        if (!HAVE_WDAY(state)) {
            /* calculate day of week (Sakamoto). The old walk started from
               first_wday_of(tm_year) - years since 1900, not the year - and
               was off for most dates. */
            static const int month_key[12] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
            int y = tm->tm_year + TM_YEAR_BASE;
            if (tm->tm_mon < 2)
                y -= 1;
            int w = (y + y / 4 - y / 100 + y / 400 + month_key[tm->tm_mon] + tm->tm_mday) % 7;
            tm->tm_wday = (w < 0) ? w + 7 : w;
            state |= S_WDAY;
        }
    }

    return (char*)bp;
}


static const unsigned char *conv_num(const unsigned char *buf, int *dest, unsigned int llim, unsigned int ulim) {
    if (!buf || !dest) {
        DATE_LOG("Error: NULL pointer passed to conv_num.\n");
        return NULL;
    }
    unsigned int result = 0;
    unsigned char ch;

    /* The limit also determines the number of valid digits. */
    unsigned int rulim = ulim;

    ch = *buf;
    if (ch < '0' || ch > '9') {
        DATE_LOG("Error: Non-numeric character found in conv_num.\n");
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
        DATE_LOG("Error: NULL pointer passed to find_string.\n");
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

/* ------------------------------------------------------------------------
 * Persian (Jalali / Solar Hijri) calendar.
 *
 * One algorithm (Kazimierz Borkowski's, the one used by jalaali-js and most
 * libraries) now drives leap years, validation and both conversions. The
 * module used to combine an astronomical leap-year approximation with an
 * unrelated 33-year conversion formula; the two disagreed (e.g. around
 * 1436/1437 and 1469/1470), so valid dates failed to convert and converted
 * dates failed validation. Supported range: Persian years 1..3177.
 * Day numbers are Julian Day Numbers (JDN), shared with the Gregorian code.
 * ---------------------------------------------------------------------- */
#define PERSIAN_MIN_YEAR 1
#define PERSIAN_MAX_YEAR 3177

static long long jal_div(long long a, long long b) { return a / b; }            /* truncating, as in the reference */
static long long jal_mod(long long a, long long b) { return a - (a / b) * b; }


static bool jal_cal(long long jy, int* leap, long long* gy, long long* march) {
    static const long long breaks[] = { -61, 9, 38, 199, 426, 686, 756, 818, 1111, 1181, 1210,
                                        1635, 2060, 2097, 2192, 2262, 2324, 2394, 2456, 3178 };
    const int bl = (int)(sizeof(breaks) / sizeof(breaks[0]));
    long long jp = breaks[0], jm, jump = 0, leapJ = -14, leapG, n;

    if (jy < jp || jy >= breaks[bl - 1]) {
        return false;
    }
    long long g = jy + 621;
    for (int i = 1; i < bl; ++i) {
        jm = breaks[i];
        jump = jm - jp;
        if (jy < jm) {
            break;
        }
        leapJ += jal_div(jump, 33) * 8 + jal_div(jal_mod(jump, 33), 4);
        jp = jm;
    }
    n = jy - jp;
    leapJ += jal_div(n, 33) * 8 + jal_div(jal_mod(n, 33) + 3, 4);
    if (jal_mod(jump, 33) == 4 && jump - n == 4) {
        leapJ += 1;
    }
    leapG = jal_div(g, 4) - jal_div((jal_div(g, 100) + 1) * 3, 4) - 150;
    if (march) {
        *march = 20 + leapJ - leapG;
    }
    if (jump - n < 6) {
        n = n - jump + jal_div(jump + 4, 33) * 33;
    }
    long long l = jal_mod(jal_mod(n + 1, 33) - 1, 4);
    if (l == -1) {
        l = 4;
    }
    if (leap) {
        *leap = (int)l;
    }
    if (gy) {
        *gy = g;
    }
    return true;
}

/* Gregorian date <-> JDN (the reference's g2d/d2g; valid for any year). */
static long long jal_g2d(long long gy, long long gm, long long gd) {
    long long d = jal_div((gy + jal_div(gm - 8, 6) + 100100) * 1461, 4)
                + jal_div(153 * jal_mod(gm + 9, 12) + 2, 5)
                + gd - 34840408;
    return d - jal_div(jal_div(gy + 100100 + jal_div(gm - 8, 6), 100) * 3, 4) + 752;
}

static void jal_d2g(long long jdn, long long* gy, long long* gm, long long* gd) {
    long long j = 4 * jdn + 139361631;
    j = j + jal_div(jal_div(4 * jdn + 183187720, 146097) * 3, 4) * 4 - 3908;
    long long i = jal_div(jal_mod(j, 1461), 4) * 5 + 308;
    *gd = jal_div(jal_mod(i, 153), 5) + 1;
    *gm = jal_mod(jal_div(i, 153), 12) + 1;
    *gy = jal_div(j, 1461) - 100100 + jal_div(8 - *gm, 6);
}

/* Persian date -> JDN, or -1 outside the supported range. */
static long long jal_j2d(long long jy, long long jm, long long jd) {
    long long gy, march;
    if (!jal_cal(jy, NULL, &gy, &march)) {
        return -1;
    }
    return jal_g2d(gy, 3, march) + (jm - 1) * 31 - jal_div(jm, 7) * (jm - 7) + jd - 1;
}

/* JDN -> Persian date; false outside the supported range. */
static bool jal_d2j(long long jdn, int* jyOut, int* jmOut, int* jdOut) {
    long long gy, gm, gd, march;
    int leap;
    jal_d2g(jdn, &gy, &gm, &gd);
    long long jy = gy - 621;
    if (!jal_cal(jy, &leap, NULL, &march)) {
        return false;
    }
    long long k = jdn - jal_g2d(gy, 3, march);
    long long jm, jd;
    if (k >= 0 && k <= 185) {          /* months 1-6: 31 days */
        jm = 1 + jal_div(k, 31);
        jd = jal_mod(k, 31) + 1;
    }
    else {
        if (k >= 0) {
            k -= 186;                  /* months 7-12 of this year */
        }
        else {                         /* before Farvardin 1: end of the previous year */
            jy -= 1;
            k += 179;
            if (leap == 1) {
                k += 1;
            }
        }
        jm = 7 + jal_div(k, 30);
        jd = jal_mod(k, 30) + 1;
    }
    if (jy < PERSIAN_MIN_YEAR || jy > PERSIAN_MAX_YEAR) {
        return false;
    }
    *jyOut = (int)jy;
    *jmOut = (int)jm;
    *jdOut = (int)jd;
    return true;
}

static bool is_persian_leap_year(int year) {
    int leap;
    if (year < PERSIAN_MIN_YEAR || year > PERSIAN_MAX_YEAR || !jal_cal(year, &leap, NULL, NULL)) {
        DATE_LOG("Error: Invalid year passed to is_persian_leap_year. Year: %d\n", year);
        return false;
    }
    return leap == 0;
}

static int persian_days_in_month(int year, int month) {
    if (year < PERSIAN_MIN_YEAR || year > PERSIAN_MAX_YEAR || month <= 0 || month > 12) {
        DATE_LOG("Error: Invalid year or month passed to persian_days_in_month. Year: %d, Month: %d\n", year, month);
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

/* Gregorian years accepted by the library: the Julian Day Number stays
   non-negative and all day arithmetic stays far from integer overflow. */
#define GREGORIAN_MIN_YEAR (-4712)
#define GREGORIAN_MAX_YEAR 1000000

static bool date_is_valid_ymd(int y, int m, int d, CalendarType type) {
    if (y == 0 || m <= 0 || m > 12 || d <= 0) {
        DATE_LOG("Error: Invalid year, month, or day in date_is_valid_ymd. Year: %d, Month: %d, Day: %d\n", y, m, d);
        return false;
    }

    if (type == Gregorian) {
        if (y < GREGORIAN_MIN_YEAR || y > GREGORIAN_MAX_YEAR) {
            DATE_LOG("Error: Gregorian year %d outside the supported range.\n", y);
            return false;
        }
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
        DATE_LOG("Error: Invalid month in days_in_month. Month: %d\n", month);
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

/* True Julian Day Number of a proleptic Gregorian date (2024-03-20 -> 2460390),
   the axis shared with the Persian conversions. 64-bit arithmetic: `365 * year`
   overflowed int for large (e.g. parsed) years, which is undefined behavior.
   (The previous version also lacked the 4800-year offset, so it was not a JDN;
   only its weekday, offset by a multiple of 7, happened to be right.) */
static long long gregorian_to_jdn(int year, int month, int day) {
    long long a = (14 - month) / 12;
    long long y = (long long)year + 4800 - a;
    long long m = month + 12 * a - 3;
    return day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
}

// Persian date -> Julian Day Number (shared day axis with gregorian_to_jdn),
// or -1 for a date outside the supported range.
static long long persian_to_jdn(int year, int month, int day) {
    return jal_j2d(year, month, day);
}

// JDN of a valid date in either calendar, or -1 when the date is invalid.
static long long date_jdn_of(const Date* date) {
    if (!date || !date_is_valid_ymd(date->year, date->month, date->day, date->calendarType)) {
        return -1;
    }
    if (date->calendarType == Gregorian) {
        return gregorian_to_jdn(date->year, date->month, date->day);
    }
    if (date->calendarType == Persian) {
        return persian_to_jdn(date->year, date->month, date->day);
    }
    return -1;
}

// Fills *out with the date of day number jdn in the requested calendar; false
// when that day is outside the calendar's supported range.
static bool date_from_jdn(long long jdn, CalendarType type, Date* out) {
    if (jdn < 0) {
        return false;
    }
    if (type == Gregorian) {
        long long gy, gm, gd;
        jal_d2g(jdn, &gy, &gm, &gd);
        if (gy < GREGORIAN_MIN_YEAR || gy > GREGORIAN_MAX_YEAR || gy == 0) {
            return false;
        }
        out->year = (int)gy;
        out->month = (int)gm;
        out->day = (int)gd;
        out->calendarType = Gregorian;
        return true;
    }
    if (type == Persian) {
        int jy, jm, jd;
        if (!jal_d2j(jdn, &jy, &jm, &jd)) {
            return false;
        }
        out->year = jy;
        out->month = jm;
        out->day = jd;
        out->calendarType = Persian;
        return true;
    }
    return false;
}

/**
 * @brief Checks if the provided Date object represents a valid date.
 *
 * This function validates the year, month, and day components of the Date object 
 * based on the specified calendar type. If the Date object is NULL, the function 
 * returns false and logs an error message.
 *
 * @param date A pointer to the Date object to validate.
 * @return true if the Date object is valid; false otherwise.
 */
bool date_is_valid(const Date* date) {
    if (!date) {
        DATE_LOG("Error: Date pointer is NULL in date_is_valid.\n");
        return false;
    }
    return date_is_valid_ymd(date->year, date->month, date->day, date->calendarType);
}

/**
 * @brief Creates a new Date object with the specified calendar type.
 *
 * This function allocates memory for a new Date object and initializes it with 
 * invalid values (-1) to signify a null date. The calendar type is set based on 
 * the provided type parameter.
 *
 * @param type The CalendarType (Gregorian or Persian) to be used for the new Date.
 * @return A pointer to the newly created Date object.
 */
Date* date_create(CalendarType type) {
    Date* date = (Date*)malloc(sizeof(Date));
    if (!date) {
        DATE_LOG("Error: Memory allocation failed in date_create.\n");
        return NULL;
    }

    // Initialize with invalid values to signify a null (invalid) date
    date->year = -1;
    date->month = -1;
    date->day = -1;
    date->calendarType = type;

    return date;
}

/**
 * @brief Creates a new Date object with the specified year, month, day, and calendar type.
 *
 * This function allocates memory for a new Date object and initializes it with the 
 * provided year, month, and day values. It validates the provided date parameters and 
 * logs an error message if they are invalid.
 *
 * @param y The year component of the date.
 * @param m The month component of the date.
 * @param d The day component of the date.
 * @param type The CalendarType (Gregorian or Persian) to be used for the new Date.
 * 
 * @return A pointer to the newly created Date object, or exits the program if the date is invalid.
 */
Date* date_create_ymd(int y, int m, int d, CalendarType type) {
    Date* date = (Date*)malloc(sizeof(Date));
    if (!date) {
        DATE_LOG("Error: Memory allocation failed in date_create_ymd.\n");
        return NULL;
    }

    if (!date_is_valid_ymd(y, m, d, type)) {
        DATE_LOG("Error: Invalid date parameters in date_create_ymd. Year: %d, Month: %d, Day: %d\n", y, m, d);
        free(date);
        return NULL;
    }

    date->year = y;
    date->month = m;
    date->day = d;
    date->calendarType = type;
    return date;
}

/**
 * @brief Checks if the provided Date object is null.
 *
 * This function checks if the Date object is NULL. If the Date object is NULL, 
 * the function logs a warning message and returns true.
 *
 * @param date A pointer to the Date object to check.
 * @return true if the Date object is NULL; false otherwise.
 */
bool date_is_null(const Date* date) {
    if (!date) {
        DATE_LOG("Warning: Date pointer is NULL in date_is_null.\n");
        return true;
    }
    return false;
}

/**
 * @brief Adds a specified number of days to a given date.
 *
 * This function creates a new Date object by adding or subtracting the specified 
 * number of days to/from the original date. It adjusts the resulting date for 
 * month and year overflows/underflows based on the calendar type.
 *
 * @param orig_date A pointer to the original Date object.
 * @param ndays The number of days to add (can be negative to subtract days).
 * 
 * @return A pointer to the new Date object with the adjusted date.
 */
Date* date_add_days(const Date* orig_date, int ndays) {
    if (!orig_date) {
        DATE_LOG("Error: Original date is NULL in date_add_days.\n");
        return NULL;
    }
    if (!date_is_valid_ymd(orig_date->year, orig_date->month, orig_date->day, orig_date->calendarType)) {
        DATE_LOG("Error: Invalid original date in date_add_days. Year: %d, Month: %d, Day: %d\n", orig_date->year, orig_date->month, orig_date->day);
        return NULL;
    }

    /* Work on the Julian Day axis: O(1) instead of a day-by-day walk, and a
       result outside the calendar's range (e.g. before Persian year 1) is
       reported as NULL instead of looping/overflowing into a garbage date. */
    long long jdn = date_jdn_of(orig_date);
    if (jdn < 0) {
        DATE_LOG("Error: Cannot map the original date to a day number in date_add_days.\n");
        return NULL;
    }

    Date* new_date = (Date*)malloc(sizeof(Date));
    if (!new_date) {
        DATE_LOG("Error: Memory allocation failed for new date in date_add_days.\n");
        return NULL;
    }

    if (!date_from_jdn(jdn + ndays, orig_date->calendarType, new_date)) {
        DATE_LOG("Error: Result of date_add_days is outside the supported range.\n");
        free(new_date);
        return NULL;
    }
    return new_date;
}

/**
 * @brief Adds a specified number of months to a given date.
 *
 * This function creates a new Date object by adding the specified number of months to the original date.
 * It adjusts the resulting date for cases such as month overflow and day overflow based on the calendar type.
 *
 * @param orig_date A pointer to the original Date object.
 * @param nmonths The number of months to add.
 * 
 * @return A pointer to the new Date object with the adjusted date.
 */
Date* date_add_months(const Date* orig_date, int nmonths) {
    if (orig_date == NULL || !date_is_valid_ymd(orig_date->year, orig_date->month, orig_date->day, orig_date->calendarType)) {
        DATE_LOG("Error: date is null or not valid in date_add_months.\n");
        return NULL;
    }

    Date* new_date = (Date*)malloc(sizeof(Date));
    if (new_date == NULL) {
        DATE_LOG("Error: Cannot allocate memory for new date in date_add_months.\n");
        return NULL;
    }

    *new_date = *orig_date;
    int years_to_add = nmonths / 12;
    nmonths %= 12;

    new_date->year += years_to_add;
    new_date->month += nmonths;

    // Both calendars have exactly 12 months per year.
    if (new_date->month > 12) {
        new_date->year++;
        new_date->month -= 12;
    }
    else if (new_date->month < 1) {
        new_date->year--;
        new_date->month += 12;
    }

    int days_this_month = days_in_month(new_date->year, new_date->month, orig_date->calendarType);
    if (new_date->day > days_this_month) {
        new_date->day = days_this_month;
    }
    return new_date;
}

/**
 * @brief Adds a specified number of years to a given date.
 *
 * This function creates a new Date object by adding the specified number of years to the original date.
 * It adjusts the resulting date for leap years, ensuring the date remains valid in the resulting year.
 *
 * @param orig_date A pointer to the original Date object.
 * @param nyears The number of years to add.
 * 
 * @return A pointer to the new Date object with the adjusted date.
 */
Date* date_add_years(const Date* orig_date, int nyears) {
    if (orig_date == NULL || !date_is_valid_ymd(orig_date->year, orig_date->month, orig_date->day, orig_date->calendarType)) {
        DATE_LOG("Error: date is null or not valid in date_add_years.\n");
        return NULL;
    }

    Date* new_date = (Date*)malloc(sizeof(Date));
    if (new_date == NULL) {
        DATE_LOG("Error: Cannot allocate memory for new date in date_add_years.\n");
        return NULL;
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

/**
 * @brief Retrieves the year, month, and day from a Date object.
 *
 * This function extracts the year, month, and day values from the specified Date object and stores them
 * in the provided pointers.
 *
 * @param date A pointer to the Date object.
 * @param year A pointer to an integer where the year value will be stored.
 * @param month A pointer to an integer where the month value will be stored.
 * @param day A pointer to an integer where the day value will be stored.
 */
void date_get_date(const Date* date, int *year, int *month, int *day) {
     if (!date) {
        DATE_LOG("Error: Passed 'date' is NULL in date_get_date.\n");
        return;
    }
    if (!year || !month || !day) {
        DATE_LOG("Error: One or more output pointers (year, month, day) are NULL in date_get_date.\n");
        return;
    }

    *year = date->year;
    *month = date->month;
    *day = date->day;
}

/**
 * @brief Retrieves the day from a Date object.
 *
 * This function returns the day of the month for the specified Date object.
 *
 * @param date A pointer to the Date object.
 * @return The day of the month, or -1 if the date is invalid or NULL.
 */
int date_day(const Date* date) {
    if (date == NULL || !date_is_valid(date)) {
        // Handle the error, as the date is NULL
        DATE_LOG("Error: Date is null in date_day or date is not valid.\n");
        return -1; // Return an invalid day
    }
    return date->day;
}

/**
 * @brief Retrieves the month from a Date object.
 *
 * This function returns the month for the specified Date object.
 *
 * @param date A pointer to the Date object.
 * @return The month, or -1 if the date is invalid or NULL.
 */
int date_month(const Date* date) {
    if (date == NULL || !date_is_valid(date)) {
        // Handle the error, as the date is NULL
        DATE_LOG("Error: Date is null in date_month or date is not valid.\n");
        return -1; // Return an invalid month
    }
    return date->month;
}

/**
 * @brief Retrieves the year from a Date object.
 *
 * This function returns the year for the specified Date object.
 *
 * @param date A pointer to the Date object.
 * @return The year, or -1 if the date is invalid or NULL.
 */
int date_year(const Date* date) {
    if (date == NULL) {
        // Handle the error, as the date is NULL
        DATE_LOG("Error: Date is null in date_year or date is not valid.\n");
        return -1; // Return an invalid year
    }
    return date->year;
}

/**
 * @brief Returns the day of the week for the given date.
 *
 * This function calculates the day of the week for the specified Date object.
 * The return value corresponds to Monday as 1 through Sunday as 7.
 *
 * @param date A pointer to the Date object.
 * @return The day of the week (Monday = 1, ..., Sunday = 7), or -1 in case of an error (NULL date or unsupported calendar type).
 */
int date_day_of_week(const Date* date) {
    if (date == NULL) {
        DATE_LOG("Error: Date is null in date_day_of_week.\n");
        return -1; // Indicate an error
    }

    long long jdn = date_jdn_of(date);
    if (jdn < 0) {
        DATE_LOG("Error: Invalid date or unsupported calendar type in date_day_of_week.\n");
        return -1; // Indicate an error (an invalid date has no weekday)
    }
    // gregorian_to_jdn / persian_to_jdn produce values where jdn % 7 == 0
    // corresponds to Monday. Map to Monday = 1, ..., Sunday = 7.
    return (int)(jdn % 7) + 1;
}

/**
 * @brief Calculates the day of the year for the given date.
 *
 * This function calculates the day of the year for the specified Date object, ranging from 1 to 365 (or 366 in a leap year).
 *
 * @param date A pointer to the Date object.
 * @return The day of the year (1 to 365/366), or -1 in case of an error (NULL date or unsupported calendar type).
 */
int date_day_of_year(const Date* date) {
    if (date == NULL) {
        DATE_LOG("Error: Date is null in date_day_of_year.\n");
        return -1; // Indicate an error
    }
    /* The month indexes the tables below: an invalid (e.g. null or parsed)
       month used to read outside them. */
    if (!date_is_valid(date)) {
        DATE_LOG("Error: Invalid date in date_day_of_year.\n");
        return -1;
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
        // Months 1-6 are 31 days each (cumulative: 31, 62, 93, 124, 155, 186),
        // months 7-11 are 30 days each (216, 246, 276, 306, 336),
        // month 12 is 29 or 30 days (leap day captured by `day` itself).
        static const int daysBeforeMonthPersian[] = {0, 31, 62, 93, 124, 155, 186, 216, 246, 276, 306, 336};
        dayOfYear = daysBeforeMonthPersian[date->month - 1] + date->day;
    }
    else {
        DATE_LOG("Error: Unsupported calendar type in date_day_of_year.\n");
        return -1; // Indicate an error for unsupported calendar types
    }

    return dayOfYear;
}

/**
 * @brief Returns the number of days in the specified month of the given year.
 *
 * This function returns the total number of days in the month for the provided Date object.
 * It accounts for leap years when calculating the number of days in February.
 *
 * @param date A pointer to the Date object.
 * @return The number of days in the month, or -1 in case of an error (e.g., NULL date or unsupported calendar type).
 */
int date_days_in_month(const Date* date) {
    if (!date) {
        DATE_LOG("Error: Date pointer is NULL in date_days_in_month.\n");
        return -1; // Indicate an error
    }
    if (date->month < 1 || date->month > 12 ||
        (date->calendarType == Persian && (date->year < PERSIAN_MIN_YEAR || date->year > PERSIAN_MAX_YEAR))) {
        DATE_LOG("Error: Invalid month/year in date_days_in_month.\n");
        return -1; // the month indexes a 12-entry table below
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
        DATE_LOG("Error: Unsupported calendar type in date_days_in_month.\n");
        return -1; // Indicate an error for unsupported calendar types
    }

    return days;
}

/**
 * @brief Returns the total number of days in the year of the given date.
 *
 * This function calculates the total number of days in the year for the specified Date object,
 * considering leap years if applicable.
 *
 * @param date A pointer to the Date object.
 * @return The total number of days in the year (365 or 366), or -1 in case of an error (e.g., NULL date or unsupported calendar type).
 */
int date_days_in_year(const Date* date) {
    if (!date) {
        DATE_LOG("Error: Date pointer is NULL in date_days_in_year.\n");
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
        DATE_LOG("Error: Unsupported calendar type in date_days_in_year.\n");
        return -1; // Indicate an error for unsupported calendar types
    }
}

/**
 * @brief Calculates the ISO 8601 week number for a given date.
 *
 * This function calculates the ISO 8601 week number for a provided date, taking into account
 * the calendar type (Gregorian or Persian). It also optionally returns the year the week belongs to.
 *
 * @param date A pointer to the Date object.
 * @param yearNumber A pointer to an integer to store the year the week belongs to (optional, can be NULL).
 * 
 * @return The ISO 8601 week number, or -1 in case of an error (e.g., unsupported calendar type or NULL date).
 */
/* Number of ISO 8601 weeks (52 or 53) in Gregorian year y: 53 when the year
   starts on a Thursday, or is a leap year starting on a Wednesday. */
static int iso_weeks_in_year(int y) {
    long long yy = y;
    long long p = (yy + yy / 4 - yy / 100 + yy / 400) % 7;
    long long q = ((yy - 1) + (yy - 1) / 4 - (yy - 1) / 100 + (yy - 1) / 400) % 7;
    if (p < 0) p += 7;
    if (q < 0) q += 7;
    return (p == 4 || q == 3) ? 53 : 52;
}

int date_week_number(const Date* date, int* yearNumber) {
    if (!date) {
        DATE_LOG("Error: Date is NULL in date_week_number.\n");
        return -1;
    }

    if (!date_is_valid(date)) {
        DATE_LOG("Error: Invalid date in date_week_number.\n");
        return -1;
    }

    int dayOfYear = date_day_of_year(date);
    int wday = date_day_of_week(date);

    // Determine the week number based on the calendar type
    int week;
    if (date->calendarType == Gregorian) {
        /* ISO 8601: week = (ordinal - weekday + 10) / 7 (Monday = 1). Days
           before week 1 belong to the last week (52 or 53) of the previous
           year, and a "week 53" in a 52-week year is week 1 of the next. */
        int isoYear = date->year;
        week = ((dayOfYear - wday + 10) / 7);
        if (week < 1) {
            isoYear = date->year - 1;
            week = iso_weeks_in_year(isoYear);
        }
        else if (week > iso_weeks_in_year(date->year)) {
            isoYear = date->year + 1;
            week = 1;
        }
        if (yearNumber != NULL) {
            *yearNumber = isoYear;
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
        DATE_LOG("Error: Unsupported calendar type in date_week_number.\n");
        return -1; // Indicate an error for unsupported calendar types
    }

    return week;
}

/**
 * @brief Calculates the number of days between two dates.
 *
 * This function calculates the number of days between two Date objects. Both dates must have
 * the same calendar type (Gregorian or Persian). The function returns the difference in days,
 * or -1 in case of an error (e.g., mismatched calendar types or invalid dates).
 *
 * @param from A pointer to the start Date object.
 * @param to A pointer to the end Date object.
 * 
 * @return The number of days between the two dates, or -1 in case of an error.
 */
int date_days_to(const Date* from, const Date* to) {
    if (!from || !to) {
        DATE_LOG("Error: One or both date pointers are NULL in date_days_to.\n");
        return -1;
    }
    if (from->calendarType != to->calendarType) {
        DATE_LOG("Error: Calendar types of dates do not match in date_days_to.\n");
        return -1;
    }
    if (!date_is_valid(from)) {
        DATE_LOG("Error: The 'from' date is invalid in date_days_to.\n");
        return -1;
    }
    if (!date_is_valid(to)) {
        DATE_LOG("Error: The 'to' date is invalid in date_days_to.\n");
        return -1;
    }

    // Convert both dates to Julian Day Number and find the difference
    long from_jd = date_to_julian_day(from);
    long to_jd = date_to_julian_day(to);

    return (int)(to_jd - from_jd);
}

/**
 * @brief Compares two dates for equality.
 *
 * This function checks if two Date objects are equal by comparing their year, month, and day fields.
 * The dates must also have the same calendar type (Gregorian or Persian).
 *
 * @param lhs A pointer to the first Date object.
 * @param rhs A pointer to the second Date object.
 * 
 * @return true if the dates are equal, false otherwise.
 */
bool date_is_equal(const Date* lhs, const Date* rhs) {
    if (lhs == NULL || rhs == NULL || !date_is_valid(lhs) || !date_is_valid(rhs) || (lhs->calendarType != rhs->calendarType)) {
        DATE_LOG("Error: One or both dates are null in date_is_equals.\n");
        return false;  // Consider unequal if either date is NULL
    }
    return (lhs->year == rhs->year) && (lhs->month == rhs->month) && (lhs->day == rhs->day);
}

/**
 * @brief Compares two dates to check if the first is less than the second.
 *
 * This function compares two Date objects and determines if the first date is earlier than the second.
 * The dates must have the same calendar type (Gregorian or Persian).
 *
 * @param lhs A pointer to the first Date object.
 * @param rhs A pointer to the second Date object.
 * 
 * @return true if the first date is earlier than the second, false otherwise.
 */
bool date_is_less_than(const Date* lhs, const Date* rhs) {
    if (lhs == NULL || rhs == NULL || !date_is_valid(lhs) || !date_is_valid(rhs) || (lhs->calendarType != rhs->calendarType)) {
        DATE_LOG("Error: One or both dates are null or invalid in date_is_less_than.\n");
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

/**
 * @brief Compares two dates to check if the first is less than or equal to the second.
 *
 * This function compares two Date objects and determines if the first date is earlier than or equal to the second.
 * The dates must have the same calendar type (Gregorian or Persian).
 *
 * @param lhs A pointer to the first Date object.
 * @param rhs A pointer to the second Date object.
 * 
 * @return true if the first date is earlier than or equal to the second, false otherwise.
 */
bool date_is_less_than_or_equal(const Date* lhs, const Date* rhs) {
    if (lhs == NULL || rhs == NULL || !date_is_valid(lhs) || !date_is_valid(rhs) || (lhs->calendarType != rhs->calendarType)) {
        DATE_LOG("Error: One or both dates are null or invalid in date_is_less_than_or_equal.\n");
        return false;  // Cannot determine if invalid or NULL
    }
    return date_is_less_than(lhs, rhs) || date_is_equal(lhs, rhs);
}

/**
 * @brief Compares two dates to check if the first is greater than the second.
 *
 * This function compares two Date objects and determines if the first date is later than the second.
 * The dates must have the same calendar type (Gregorian or Persian).
 *
 * @param lhs A pointer to the first Date object.
 * @param rhs A pointer to the second Date object.
 * @return true if the first date is later than the second, false otherwise.
 */
bool date_is_greater_than(const Date* lhs, const Date* rhs) {
    if (lhs == NULL || rhs == NULL || !date_is_valid(lhs) || !date_is_valid(rhs) || (lhs->calendarType != rhs->calendarType)) {
        DATE_LOG("Error: One or both dates are null or invalid in date_is_greater_than.\n");
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

/**
 * @brief Compares two dates to check if the first is greater than or equal to the second.
 *
 * This function compares two Date objects and determines if the first date is later than or equal to the second.
 * The dates must have the same calendar type (Gregorian or Persian).
 *
 * @param lhs A pointer to the first Date object.
 * @param rhs A pointer to the second Date object.
 * 
 * @return true if the first date is later than or equal to the second, false otherwise.
 */
bool date_is_greater_than_or_equal(const Date* lhs, const Date* rhs) {
    if (lhs == NULL || rhs == NULL || !date_is_valid(lhs) || !date_is_valid(rhs) || (lhs->calendarType != rhs->calendarType)) {
        DATE_LOG("Error: One or both dates are null or invalid in date_is_greater_than_or_equal.\n");
        return false;  // Cannot determine if invalid or NULL
    }
    return date_is_greater_than(lhs, rhs) || date_is_equal(lhs, rhs);
}

/**
 * @brief Compares two Date objects for inequality.
 *
 * This function checks if two Date objects are not equal. The comparison takes into account
 * the year, month, and day of the dates. It returns true if the dates are not equal.
 *
 * @param lhs A pointer to the first Date object.
 * @param rhs A pointer to the second Date object.
 * @return true if the dates are not equal, false otherwise.
 */
bool date_is_not_equals(const Date* lhs, const Date* rhs) {
    return !date_is_equal(lhs, rhs);
}

/**
 * @brief Checks if a given year is a leap year for a specific calendar type.
 *
 * This function determines if the provided year is a leap year according to the specified
 * calendar type (Gregorian or Persian). It handles both the Gregorian and Persian leap year
 * rules and returns true if the year is a leap year.
 *
 * @param year The year to check.
 * @param type The calendar type (Gregorian or Persian).
 * @return true if the year is a leap year, false otherwise.
 */
bool date_is_leap_year_y(int year, CalendarType type) {
    if (type != Gregorian && type != Persian) {
        DATE_LOG("Error: CalendarType is not valid in date_is_leap_year_y.\n");
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

/**
 * @brief Checks if the year of a given Date object is a leap year.
 *
 * This function determines if the year of the provided Date object is a leap year based on
 * the date's calendar type (Gregorian or Persian).
 *
 * @param date A pointer to the Date object.
 * @return true if the year is a leap year, false otherwise.
 */
bool date_is_leap_year(const Date* date) {
    if (date == NULL ) {
        DATE_LOG("Error: Date is null in date_is_leap_year");
        return false;  // Cannot determine if the date is NULL
    }
    return date_is_leap_year_y(date->year, date->calendarType);
}

/**
 * @brief Sets the date of a Date object.
 *
 * This function sets the year, month, day, and calendar type of a Date object. It also validates
 * the input parameters to ensure that the date is valid for the specified calendar type.
 *
 * @param date A pointer to the Date object to be set.
 * @param year The year to set.
 * @param month The month to set (1-12).
 * @param day The day to set (1-31 depending on the month and calendar type).
 * @param type The calendar type (Gregorian or Persian).
 * 
 * @return true if the date was successfully set, false otherwise.
 */
bool date_set_date(Date* date, int year, int month, int day, CalendarType type) {
    if (date == NULL) {
        DATE_LOG("Error: Date is null in date_set_date");
        return false;
    }
    if (year <= 0 || month <= 0 || month > 12 || day <= 0) {
        DATE_LOG("Error: Year, month, or day are not valid");
        return false;
    }

    // Check validity based on calendar type
    if ((type == Gregorian && day > days_in_month(year, month, Gregorian)) ||
        (type == Persian && day > days_in_month(year, month, Persian))) {
        DATE_LOG("Error: Day is not valid for the given month and year");
        return false;
    }

    date->year = year;
    date->month = month;
    date->day = day;
    date->calendarType = type;

    return true;
}

/**
 * @brief Gets the current date based on the specified calendar type.
 *
 * This function retrieves the current date and returns it as a Date object. The calendar type
 * can be either Gregorian or Persian. If Persian is selected, the function converts the current
 * Gregorian date to the equivalent Persian date.
 *
 * @param type The calendar type (Gregorian or Persian).
 * @return A pointer to the newly allocated Date object representing the current date. The caller is responsible for freeing the memory. Returns NULL if memory allocation fails or if an unsupported calendar type is provided.
 */
Date* date_current_date(CalendarType type) {
    // Get the current time. localtime() shares a static struct tm (a data race
    // under concurrent use) and can return NULL; use the reentrant variant and
    // guard the result.
    time_t now = time(NULL);
    struct tm tmbuf;
#if defined(_WIN32) || defined(_WIN64)
    struct tm* current = (localtime_s(&tmbuf, &now) == 0) ? &tmbuf : NULL;
#else
    struct tm* current = localtime_r(&now, &tmbuf);
#endif
    if (!current) {
        DATE_LOG("Error: localtime_r/localtime_s failed in date_current_date.\n");
        return NULL;
    }

    // Allocate memory for a new Date
    Date* currentDate = (Date*)malloc(sizeof(Date));
    if (currentDate == NULL) {
        DATE_LOG("Error: Memory allocation failed in date_current_date.\n");
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
            DATE_LOG("Error: Failed to get current Persian Date in date_current_date.\n");
            free(currentDate);
            return NULL;
        }
    } 
    else {
        DATE_LOG("Error: Unsupported calendar type in date_current_date.\n");
        free(currentDate);
        return NULL;
    }

    return currentDate;
}

/**
 * @brief Creates a Date object from a string representation of the date.
 *
 * This function parses a date string according to the provided format and calendar type,
 * and creates a corresponding Date object. It supports both Gregorian and Persian calendars.
 *
 * @param string The string containing the date to be parsed.
 * @param format The format string specifying the date format.
 * @param type The calendar type (Gregorian or Persian).
 * 
 * @return A pointer to the newly allocated Date object. The caller is responsible for freeing the memory. 
 * Returns NULL if parsing fails or if memory allocation fails.
 */
Date* date_from_string(const char* string, const char* format, CalendarType type) {
    if (string == NULL || format == NULL) {
        DATE_LOG("Error: Invalid argument passed to date_from_string");
        return NULL;
    }

    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));

    // Allocate memory for a new Date
    Date* date = (Date*)malloc(sizeof(Date));
    if (date == NULL) {
        DATE_LOG("Error: Memory allocation failed in date_from_string.\n");
        return NULL;
    }

    date->calendarType = type;

    if (type == Gregorian) {
        #if defined(_WIN32) || defined(_WIN64)
        if (win_strptime(string, format, &tm) == NULL) {
            DATE_LOG("Error: failed to parse string in date_from_string.\n");
            free(date);
            return NULL; // Failed to parse string
        }
        #else 
        if (strptime(string, format, &tm) == NULL) {
            DATE_LOG("Error: failed to parse string in date_from_string.\n");
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
            DATE_LOG("Error: failed to parse string in date_from_string.\n");
            free(date);
            return NULL; // Failed to parse string
        }

    }
    else {
        DATE_LOG("Error: Unsupported calendar type in date_from_string.\n");
        free(date);
        return NULL;
    }

    /* A parsed but impossible date (month 13, day 0 from a partial format,
       an unbounded year, ...) is a parse failure: returning it let later calls
       index month tables out of bounds or overflow day arithmetic. */
    if (!date_is_valid(date)) {
        DATE_LOG("Error: parsed date is not a valid date in date_from_string.\n");
        free(date);
        return NULL;
    }

    return date;
}

/**
 * @brief Converts a Date object to a formatted string.
 *
 * This function takes a Date object and formats it as a string according to the provided format.
 * The function supports both Gregorian and Persian calendars. The format string follows the
 * rules of the `strftime` function for Gregorian dates and a simple formatting rule for Persian dates.
 *
 * @param date A pointer to the Date object to be formatted.
 * @param format A format string that specifies how to format the date.
 * @return A pointer to a dynamically allocated string containing the formatted date. 
 * The caller is responsible for freeing the memory. Returns NULL if the input date or format is NULL, 
 * or if memory allocation fails.
 */
char* date_to_string(const Date* date, const char* format) {
    if (!date || !format) {
        DATE_LOG("Error: Invalid argument passed to date_to_string. Date or format is NULL.\n");
        return NULL;
    }

    // Allocate buffer for the formatted date string. 256 leaves headroom for
    // %c-style strftime output (e.g. "Thursday, February 29, 2024" + locale).
    const size_t buf_size = 256;
    char* date_str = (char*)malloc(buf_size * sizeof(char));
    if (!date_str) {
        DATE_LOG("Error: Memory allocation failed for date string in date_to_string.\n");
        return NULL;
    }

    if (date->calendarType == Gregorian) {
        // Prepare a struct tm from the Date struct for Gregorian calendar
        struct tm tm_date = {
            .tm_sec = 0,
            .tm_min = 0,
            .tm_hour = 0,
            .tm_mday = date->day,
            .tm_mon = date->month - 1,
            .tm_year = date->year - 1900,
            .tm_isdst = -1
        };
        /* strftime's %a/%A/%u/%w/%j/%U/%W read these; left at 0 they printed
           "Sunday" and day 001 for every date. */
        int dow = date_day_of_week(date);      /* Monday = 1 .. Sunday = 7 */
        int doy = date_day_of_year(date);
        if (dow > 0 && doy > 0) {
            tm_date.tm_wday = dow % 7;         /* Sunday = 0 */
            tm_date.tm_yday = doy - 1;
        }

        if (strftime(date_str, buf_size, format, &tm_date) == 0) {
            DATE_LOG("Error: Failed to format date in date_to_string.\n");
            free(date_str);
            return NULL;
        }
    }
    else if (date->calendarType == Persian) {
        int result = snprintf(date_str, buf_size, "%04d-%02d-%02d", date->year, date->month, date->day);
        if (result < 0 || (size_t)result >= buf_size) {
            DATE_LOG("Error: Failed to format Persian date in date_to_string.\n");
            free(date_str);
            return NULL;
        }
    }
    else {
        DATE_LOG("Error: Unsupported calendar type in date_to_string.\n");
        free(date_str);
        return NULL;
    }

    return date_str;
}

/**
 * @brief Converts a Date object to a Julian Day Number.
 *
 * This function converts a given date in the Gregorian calendar to the corresponding Julian Day Number (JDN).
 * The Julian Day Number is the continuous count of days since the beginning of the Julian Period used primarily
 * by astronomers.
 *
 * @param date A pointer to the Date object to be converted.
 * @return The Julian Day Number corresponding to the given date. Returns -1 if the input date is NULL or invalid.
 */
long date_to_julian_day(const Date* date) {
    if (!date) {
        DATE_LOG("Error: Date is NULL in date_to_julian_day.\n");
        return -1;
    }
    if (!date_is_valid(date)) {
        DATE_LOG("Error: Invalid date in date_to_julian_day.\n");
        return -1;
    }

    /* Both calendars share one day axis (the Julian Day Number), so
       `date_days_to`, sorting by JDN and cross-calendar code agree. The
       arithmetic is 64-bit; the supported year ranges keep it within long. */
    long long jd = date_jdn_of(date);
    return (jd < 0) ? -1 : (long)jd;
}

/**
 * @brief Creates a Date object from a Julian Day Number.
 *
 * This function converts a Julian Day Number (JDN) to the corresponding date in the Gregorian calendar.
 * It allocates a new Date structure and returns a pointer to it.
 *
 * @param jd The Julian Day Number to be converted.
 * @return A pointer to the newly allocated Date object. Returns NULL if the Julian Day Number is invalid or 
 * if memory allocation fails.
 */
Date* date_from_julian_day(long jd) {
    if (jd < 0) {
        DATE_LOG("Error: Invalid Julian Day Number in date_from_julian_day.\n");
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
        DATE_LOG("Error: Memory allocation failed for date in date_from_julian_day.\n");
        return NULL;
    }

    date->year = y - 4800 + (m + 2) / 12;
    date->month = (m + 2) % 12 + 1;
    date->day = d + 1;
    date->calendarType = Gregorian;  // JDN -> Date result is always Gregorian.

    return date;
}

/**
 * @brief Convert a Date to Unix time (seconds since 1970-01-01T00:00:00 UTC).
 *
 * A Date carries no time-of-day, so the result is the epoch second at UTC
 * midnight of that calendar day. Works for both Gregorian and Persian dates
 * (Persian is routed through the shared Julian-Day axis), and yields negative
 * values for days before 1970. The returned value is always a multiple of 86400.
 *
 * @param date The date to convert.
 * @return Seconds since the Unix epoch at UTC midnight, or `INT64_MIN` if `date`
 *         is NULL or invalid.
 */
int64_t date_to_unix(const Date* date) {
    if (!date || !date_is_valid(date)) {
        DATE_LOG("Error: NULL/invalid date in date_to_unix.\n");
        return INT64_MIN;
    }
    long jdn = date_to_julian_day(date);
    if (jdn < 0) {
        return INT64_MIN;
    }
    /* JDN of 1970-01-01 is 2440588. */
    int64_t days = (int64_t)jdn - 2440588LL;
    return days * 86400LL;
}

/**
 * @brief Build a Date from Unix time, taking the calendar day at UTC.
 *
 * Inverse of `date_to_unix`: the time-of-day part of `unix_seconds` is dropped
 * (floored to the start of the UTC day) and the resulting calendar day is
 * returned in the requested calendar. Negative inputs (pre-1970) use floor
 * division so the day boundary is correct.
 *
 * @param unix_seconds Seconds since the Unix epoch.
 * @param type         Desired calendar of the result (Gregorian or Persian).
 * @return Newly-allocated Date (caller frees with `date_deallocate`), or NULL on
 *         an out-of-range/unsupported input or allocation/conversion failure.
 */
Date* date_from_unix(int64_t unix_seconds, CalendarType type) {
    /* Floor-divide to the UTC day; C integer division truncates toward zero, so
       adjust for a negative remainder. */
    int64_t days = unix_seconds / 86400LL;
    if (unix_seconds % 86400LL != 0 && unix_seconds < 0) {
        days -= 1;
    }
    int64_t jdn64 = days + 2440588LL;
    /* Keep within `long` (32-bit on LLP64 Windows) and the JDN domain. */
    if (jdn64 < 0 || jdn64 > 2147483647LL) {
        DATE_LOG("Error: unix_seconds out of representable range in date_from_unix.\n");
        return NULL;
    }

    Date* g = date_from_julian_day((long)jdn64);
    if (!g) {
        DATE_LOG("Error: date_from_julian_day failed in date_from_unix.\n");
        return NULL;
    }
    if (type == Gregorian) {
        return g;
    }
    if (type == Persian) {
        Date* p = date_gregorian_to_solar(g);
        free(g);
        return p;   /* NULL on conversion failure is propagated */
    }
    free(g);
    DATE_LOG("Error: unsupported calendar type in date_from_unix.\n");
    return NULL;
}

/**
 * @brief Converts a Gregorian date to a Persian (Jalali) date.
 *
 * This function takes a Gregorian date and converts it to the corresponding date
 * in the Persian calendar. It allocates a new Date structure and returns a pointer
 * to it.
 *
 * @param gregorian_date A pointer to the Gregorian date to be converted.
 * @return A pointer to the newly allocated Persian date. Returns NULL if the input
 * date is NULL or if memory allocation fails.
 */
Date* date_gregorian_to_solar(const Date* gregorian_date) {
    if (!gregorian_date) {
        DATE_LOG("Error: The provided Gregorian date is NULL in date_gregorian_to_solar.\n");
        return NULL;
    }
    /* Validate first: an out-of-range month used to index a 12-entry table. */
    if (gregorian_date->calendarType != Gregorian || !date_is_valid(gregorian_date)) {
        DATE_LOG("Error: Invalid Gregorian date in date_gregorian_to_solar.\n");
        return NULL;
    }

    Date* solar_date = (Date*)malloc(sizeof(Date));
    if (!solar_date) {
        DATE_LOG("Error: Memory allocation failed in date_gregorian_to_solar.\n");
        return NULL;
    }
    if (!date_from_jdn(gregorian_to_jdn(gregorian_date->year, gregorian_date->month, gregorian_date->day), Persian, solar_date)) {
        DATE_LOG("Error: Date is outside the Persian calendar's supported range.\n");
        free(solar_date);
        return NULL;
    }
    return solar_date;
}

/**
 * @brief Converts a Persian (Jalali) date to a Gregorian date.
 *
 * This function takes a Persian date and converts it to the corresponding date
 * in the Gregorian calendar. It allocates a new Date structure and returns a pointer
 * to it.
 *
 * @param solar_date A pointer to the Persian date to be converted.
 * 
 * @return A pointer to the newly allocated Gregorian date. Returns NULL if the input date is NULL or if memory allocation fails.
 */
Date* date_solar_to_gregorian(const Date* solar_date) {
    if (!solar_date) {
        DATE_LOG("Error: The provided Persian date is NULL in date_solar_to_gregorian.\n");
        return NULL;
    }
    if (solar_date->calendarType != Persian || !date_is_valid(solar_date)) {
        DATE_LOG("Error: Invalid Persian date in date_solar_to_gregorian.\n");
        return NULL;
    }

    Date* gregorian_new_date = (Date*)malloc(sizeof(Date));
    if (!gregorian_new_date) {
        DATE_LOG("Error: Memory allocation failed in date_solar_to_gregorian.\n");
        return NULL;
    }
    if (!date_from_jdn(persian_to_jdn(solar_date->year, solar_date->month, solar_date->day), Gregorian, gregorian_new_date)) {
        free(gregorian_new_date);
        return NULL;
    }
    return gregorian_new_date;
}

/**
 * @brief Deallocates a Date structure.
 *
 * This function frees the memory associated with a Date structure. If the
 * provided date pointer is NULL, a warning message is printed and the function
 * returns without performing any deallocation.
 *
 * @param date A pointer to the Date structure to be deallocated.
 */
void date_deallocate(Date* date) {
    if (!date) {
        DATE_LOG("Warning: Attempted to deallocate a NULL date in date_deallocate.\n");
        return;
    }
    free(date);
}


/**
 * @brief Creates an independent deep copy of a Date object.
 *
 * The returned object owns its memory and must be freed by the caller via
 * @ref date_deallocate. Modifying the clone does not affect the source.
 *
 * @code
 * Date* a = date_create_ymd(2024, 5, 15, Gregorian);
 * Date* b = date_clone(a);
 * // b is an independent copy; date_set_date(b, ...) leaves a untouched.
 * date_deallocate(a);
 * date_deallocate(b);
 * @endcode
 *
 * @param date Source date. May be NULL.
 * @return Newly-allocated copy, or NULL if @p date is NULL or allocation fails.
 */
Date* date_clone(const Date* date) {
    if (!date) {
        DATE_LOG("Error: Date is NULL in date_clone.\n");
        return NULL;
    }
    Date* copy = (Date*)malloc(sizeof(Date));
    if (!copy) {
        DATE_LOG("Error: Memory allocation failed in date_clone.\n");
        return NULL;
    }
    *copy = *date;
    return copy;
}

/**
 * @brief Three-way comparison of two dates that share a calendar.
 *
 * Convenient for sort comparators and cmp-style code:
 *
 * @code
 * int cmp = date_compare(lhs, rhs);
 * if (cmp == INT_MIN) { ... error ... }
 * else if (cmp < 0) { ... lhs earlier ... }
 * else if (cmp > 0) { ... lhs later ... }
 * else { ... same date ... }
 * @endcode
 *
 * @param lhs Left operand. Must be non-NULL.
 * @param rhs Right operand. Must be non-NULL with the same CalendarType.
 * @return -1, 0, or 1; INT_MIN on NULL input or calendar mismatch.
 */
int date_compare(const Date* lhs, const Date* rhs) {
    if (!lhs || !rhs) {
        DATE_LOG("Error: NULL operand in date_compare.\n");
        return INT_MIN;
    }
    if (lhs->calendarType != rhs->calendarType) {
        DATE_LOG("Error: Calendar mismatch in date_compare.\n");
        return INT_MIN;
    }
    if (!date_is_valid(lhs) || !date_is_valid(rhs)) {
        DATE_LOG("Error: Invalid date in date_compare.\n");
        return INT_MIN;
    }
    if (lhs->year != rhs->year) {
        return lhs->year < rhs->year ? -1 : 1;
    }
    if (lhs->month != rhs->month) {
        return lhs->month < rhs->month ? -1 : 1;
    }
    if (lhs->day != rhs->day) {
        return lhs->day < rhs->day ? -1 : 1;
    }
    return 0;
}

/**
 * @brief Returns a newly-allocated clone of the earlier of two dates.
 *
 * If exactly one input is NULL, the other is cloned. If both are NULL,
 * returns NULL. If both are non-NULL they must share the same
 * CalendarType; otherwise NULL is returned. When the two operands are
 * equal, a clone of @p lhs is returned.
 *
 * @code
 * Date* a = date_create_ymd(2024, 5, 15, Gregorian);
 * Date* b = date_create_ymd(2024, 6,  1, Gregorian);
 * Date* earliest = date_min(a, b);   // → 2024-05-15
 * @endcode
 */
Date* date_min(const Date* lhs, const Date* rhs) {
    if (!lhs && !rhs) {
        return NULL;
    }
    if (!lhs) {
        return date_clone(rhs);
    }
    if (!rhs) {
        return date_clone(lhs);
    }
    if (lhs->calendarType != rhs->calendarType) {
        DATE_LOG("Error: Calendar mismatch in date_min.\n");
        return NULL;
    }
    int cmp = date_compare(lhs, rhs);
    if (cmp == INT_MIN) {
        return NULL;
    }
    return cmp <= 0 ? date_clone(lhs) : date_clone(rhs);
}

/**
 * @brief Returns a newly-allocated clone of the later of two dates.
 *
 * Mirror of @ref date_min for NULL/calendar-mismatch semantics. When the
 * inputs are equal, a clone of @p lhs is returned.
 *
 * @code
 * Date* latest = date_max(a, b);     // → 2024-06-01 in the example above
 * @endcode
 */
Date* date_max(const Date* lhs, const Date* rhs) {
    if (!lhs && !rhs) {
        return NULL;
    }
    if (!lhs) {
        return date_clone(rhs);
    }
    if (!rhs) {
        return date_clone(lhs);
    }
    if (lhs->calendarType != rhs->calendarType) {
        DATE_LOG("Error: Calendar mismatch in date_max.\n");
        return NULL;
    }
    int cmp = date_compare(lhs, rhs);
    if (cmp == INT_MIN) {
        return NULL;
    }
    return cmp >= 0 ? date_clone(lhs) : date_clone(rhs);
}

/**
 * @brief Returns the first day of the month containing the given date.
 *
 * The returned object is a newly-allocated Date with the same year, month,
 * and calendar as @p date and day = 1.
 *
 * @code
 * Date* mid = date_create_ymd(2024, 5, 15, Gregorian);
 * Date* first = date_first_day_of_month(mid);  // 2024-05-01
 * @endcode
 */
Date* date_first_day_of_month(const Date* date) {
    if (!date || !date_is_valid(date)) {
        DATE_LOG("Error: NULL/invalid date in date_first_day_of_month.\n");
        return NULL;
    }
    return date_create_ymd(date->year, date->month, 1, date->calendarType);
}

/**
 * @brief Returns the last day of the month containing the given date.
 *
 * Leap-year rules apply: Gregorian February returns 28 or 29; Persian
 * Esfand returns 29 or 30.
 *
 * @code
 * Date* leap = date_create_ymd(2024, 2, 1, Gregorian);
 * Date* end  = date_last_day_of_month(leap);   // 2024-02-29
 * @endcode
 */
Date* date_last_day_of_month(const Date* date) {
    if (!date || !date_is_valid(date)) {
        DATE_LOG("Error: NULL/invalid date in date_last_day_of_month.\n");
        return NULL;
    }
    int last = days_in_month(date->year, date->month, date->calendarType);
    if (last <= 0) {
        return NULL;
    }
    return date_create_ymd(date->year, date->month, last, date->calendarType);
}

/**
 * @brief Returns the calendar quarter (1..4) of the given date.
 *
 * Months 1..3 belong to Q1, 4..6 to Q2, 7..9 to Q3, 10..12 to Q4. The
 * mapping applies identically to Gregorian and Persian dates. For Persian
 * dates this aligns with the four traditional seasons (Bahar, Tabestan,
 * Paeez, Zemestan).
 *
 * @code
 * Date* d = date_create_ymd(2024, 7, 4, Gregorian);
 * int q = date_quarter(d);  // 3
 * @endcode
 *
 * @return 1..4 on success, or -1 if @p date is NULL/invalid.
 */
int date_quarter(const Date* date) {
    if (!date || !date_is_valid(date)) {
        DATE_LOG("Error: NULL/invalid date in date_quarter.\n");
        return -1;
    }
    return ((date->month - 1) / 3) + 1;
}

/**
 * @brief Returns true if the given date falls on a weekend.
 *
 * The convention differs by calendar:
 *   - Gregorian: Saturday OR Sunday counts as weekend (typical Western week).
 *   - Persian:   Friday only (Iran's official weekly holiday since 1979).
 *
 * @code
 * Date* d = date_create_ymd(2024, 1, 6, Gregorian);  // a Saturday
 * if (date_is_weekend(d)) { ... }
 * @endcode
 *
 * @return true on weekend, false on weekday or invalid input.
 */
bool date_is_weekend(const Date* date) {
    if (!date || !date_is_valid(date)) {
        DATE_LOG("Error: NULL/invalid date in date_is_weekend.\n");
        return false;
    }
    int dow = date_day_of_week(date);  /* Monday=1 .. Sunday=7 */
    if (dow < 1 || dow > 7) {
        return false;
    }
    if (date->calendarType == Gregorian) {
        return dow == 6 || dow == 7;   /* Saturday or Sunday */
    }
    /* Persian: Friday only */
    return dow == 5;
}

/**
 * @brief Returns the English weekday name as a static, non-owning string.
 *
 * The pointer is into a private static table — do not free it and do not
 * cache it past changes to the source date, since the same date may be
 * referenced again with a different value.
 *
 * @code
 * Date* d = date_create_ymd(2024, 1, 1, Gregorian);
 * printf("%s\n", date_weekday_name(d));   // "Monday"
 * @endcode
 *
 * @return "Monday"..."Sunday" on success, or NULL on NULL/invalid input.
 */
const char* date_weekday_name(const Date* date) {
    static const char* const names[7] = {
        "Monday", "Tuesday", "Wednesday", "Thursday",
        "Friday", "Saturday", "Sunday"
    };
    if (!date || !date_is_valid(date)) {
        DATE_LOG("Error: NULL/invalid date in date_weekday_name.\n");
        return NULL;
    }
    int dow = date_day_of_week(date);  /* 1..7 */
    if (dow < 1 || dow > 7) {
        return NULL;
    }
    return names[dow - 1];
}

/**
 * @brief Returns the month name as a static, non-owning string.
 *
 * Gregorian dates produce English month names ("January"..."December").
 * Persian dates produce standard transliterations
 * ("Farvardin", "Ordibehesht", "Khordad", "Tir", "Mordad", "Shahrivar",
 *  "Mehr", "Aban", "Azar", "Dey", "Bahman", "Esfand").
 *
 * @code
 * Date* p = date_create_ymd(1403, 1, 1, Persian);
 * printf("%s\n", date_month_name(p));   // "Farvardin"
 * @endcode
 *
 * @return Month name on success, or NULL on NULL/invalid input.
 */
const char* date_month_name(const Date* date) {
    static const char* const gregorian_months[12] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    static const char* const persian_months[12] = {
        "Farvardin", "Ordibehesht", "Khordad", "Tir", "Mordad", "Shahrivar",
        "Mehr", "Aban", "Azar", "Dey", "Bahman", "Esfand"
    };
    if (!date || !date_is_valid(date)) {
        DATE_LOG("Error: NULL/invalid date in date_month_name.\n");
        return NULL;
    }
    int idx = date->month - 1;
    if (idx < 0 || idx > 11) {
        return NULL;
    }
    return date->calendarType == Persian
        ? persian_months[idx]
        : gregorian_months[idx];
}
