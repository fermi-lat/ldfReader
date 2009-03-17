#ifndef EBFDEBUG_H
#define EBFDEBUG_H 1

/** @class EbdDebug
@brief Keep a package-wide debug on/off variable since Gaudi service isn't available here.

Although this class is actually publicly accessible outside
the scope of the ldfReader package, the intention is to 
keep it private to the package.
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/EbfDebug.h,v 1.1.1.1 2004/04/15 20:02:22 heather Exp $
*/

class EbfDebug {
public:

    /// Turn on or off debug output.  Returns old value of flag
    static int setDebug(int debugOn);

    static int getDebug() {return m_debug;}

    typedef enum {
       NONE = 0,
       ERRMSG = 1,
       ALL = 4
    } Verbosity;

private:
    static int m_debug;
};

#endif

