/**
 *  Time table
    ╔═════════════╦════════════╦═════╦═══════════════╦══════════════════╗
    ║ Date        ║ MJD        ║ NTP ║ NTP Timestamp ║ Epoch            ║
    ║             ║            ║ Era ║ Era Offset    ║                  ║
    ╠═════════════╬════════════╬═════╬═══════════════╬══════════════════╣
    ║ 1 Jan -4712 ║ -2,400,001 ║ -49 ║ 1,795,583,104 ║ 1st day Julian   ║
    ║ 1 Jan -1    ║ -679,306   ║ -14 ║ 139,775,744   ║ 2 BCE            ║
    ║ 1 Jan 0     ║ -678,491   ║ -14 ║ 171,311,744   ║ 1 BCE            ║
    ║ 1 Jan 1     ║ -678,575   ║ -14 ║ 202,939,144   ║ 1 CE             ║
    ║ 4 Oct 1582  ║ -100,851   ║ -3  ║ 2,873,647,488 ║ Last day Julian  ║
    ║ 15 Oct 1582 ║ -100,840   ║ -3  ║ 2,874,597,888 ║ First day        ║
    ║             ║            ║     ║               ║ Gregorian        ║
    ║ 31 Dec 1899 ║ 15019      ║ -1  ║ 4,294,880,896 ║ Last day NTP Era ║
    ║             ║            ║     ║               ║ -1               ║
    ║ 1 Jan 1900  ║ 15020      ║ 0   ║ 0             ║ First day NTP    ║
    ║             ║            ║     ║               ║ Era 0            ║
    ║ 1 Jan 1970  ║ 40,587     ║ 0   ║ 2,208,988,800 ║ First day UNIX   ║
    ║ 1 Jan 1972  ║ 41,317     ║ 0   ║ 2,272,060,800 ║ First day UTC    ║
    ║ 31 Dec 1999 ║ 51,543     ║ 0   ║ 3,155,587,200 ║ Last day 20th    ║
    ║             ║            ║     ║               ║ Century          ║
    ║ 8 Feb 2036  ║ 64,731     ║ 1   ║ 63,104        ║ First day NTP    ║
    ║             ║            ║     ║               ║ Era 1            ║
    ╚═════════════╩════════════╩═════╩═══════════════╩══════════════════╝
 */
// more details at https://en.wikipedia.org/wiki/Epoch_(reference_date)

// unix time 
// difference between 1970-01-01 00:00:00 and 2000-01-01 00:00:00
// is 946684800

#ifndef CURRENTTIME_H
#define CURRENTTIME_H
#define TIME_DIFF_2000 946684800

class CURRENTTIME {
  public:
  CURRENTTIME(void);
  int getLocalTime(void);
};

#endif
