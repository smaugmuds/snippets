/***************************************************************************
 * This code may be used freely within any non-commercial MUD, all I ask   *
 * is that these comments remain in tact and that you give me any feedback *
 * or bug reports you come up with.                                        *
 *                                  -- Midboss (eclipsing.souls@gmail.com) *
 ***************************************************************************/

/***************************************************************************
 * Add the line - #include "util.h" - to the file, or put these in merc.h. *
 ***************************************************************************/

/*
 * Returns what percentage X is of Y.
 * Example: pain_pct = percent_of (dam, ch->max_hit);
 */
#define percent_of(x,y) (((x) * 100) / (y))

/*
 * Returns X percent of Y.
 * Example: heal = times_pcnt (35, victim->max_hit);
 */
#define times_pcnt(x,y) (((y) * (x)) / 100)

/*
 * Sends the error message str to ch and returns.
 * I use this so I have less to type with multiple possible command failures.
 */
#define cmd_error(ch,str)           \
{                                   \
    send_to_char ((str), (ch));     \
    return;                         \
}

/*
 * I've written code on several different MUDs, all at the same time,
 * so I often forget which MUDs are setting which strings to NULL or
 * '\0', as I'm not the only one coding on most of them.  Anyway,
 * this will check for either type of effectively blank string.
 */
#define IS_NULL(str) ((str)[0] == '\0' || (str) == NULL)

/*
 * These are macros relating to stock ROM's daze/wait system.  If you're
 * a newbie, and haven't prodded your code much, wait is the lag time used
 * by skills and spells.  DAZED returns true if ch is dazed by a skill or
 * spell, such as bash, WAITING returns true if ch is recoiling from a skill
 * or spell, and clear_lag wipes both.  I would use clear_lag for special
 * abilities relating to wait time...  A Final Fantasy Tactics-like 'Quick'
 * spell, for instance.
 */
#define DAZED(ch)   ((ch)->daze > 0)
#define WAITING(ch) ((ch)->wait > 0)
#define clear_lag(ch)   \
{                       \
    (ch)->wait = 0;     \
    (ch)->daze = 0;     \
}

/*
 * This returns the bar section of a gauge, but leaves the caps and colors to
 * the surrounding code.  Best called within sprintf/printf_to_char.  Works
 * well with the percentage macros above.
 */
#define gauge(pct) ((pct) < 9 ?  "          " : \
                    (pct) < 19 ? "-         " : \
                    (pct) < 29 ? "--        " : \
                    (pct) < 39 ? "---       " : \
                    (pct) < 49 ? "----      " : \
                    (pct) < 59 ? "-----     " : \
                    (pct) < 69 ? "------    " : \
                    (pct) < 79 ? "-------   " : \
                    (pct) < 89 ? "--------  " : \
                    (pct) < 99 ? "--------- " : \
                                 "----------")

/*
 * This macro saves some time getting numerical args.  ch, of course, is the
 * character executing the command, arg is the argument to pass, argname is
 * the name of what the argument is to represent, var is the local variable
 * to store the integer in, err is the error message to send if the argument
 * isn't a number or doesn't exist, hi is the high range, low is the low
 * range.  Nothing too impressive, but it does save me some repetitive work.
 */
#define get_num_arg(ch, arg, argname, var, err, low, hi)        \
{                                                               \
    if (!is_number ((arg)) || (arg)[0] == '\0')                 \
    {                                                           \
        send_to_char ((err), (ch));                             \
        return;                                                 \
    }                                                           \
    (var) = atoi ((arg));                                       \
    if ((var) > (hi) || (var) < (low))                          \
    {                                                           \
        printf_to_char ((ch), "%s must be between %d and %d.",  \
                        (argname), (hi), (low));                \
        return;                                                 \
    }                                                           \
}
