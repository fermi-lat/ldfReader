#ifndef EBFDEBUG_H
#define EBFDEBUG_H 1

/** @class EbdDebug
@brief Keep a package-wide debug on/off variable since Gaudi service isn't available here.

Although this class is actually publicly accessible outside
the scope of the ldfReader package, the intention is to 
keep it private to the package.
$Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/EbfDebug.h,v 1.1 2003/05/20 16:10:08 jrb Exp $
*/

class EbfDebug {
public:

    /// Turn on or off debug output.  Returns old value of flag
    static bool setDebug(bool debugOn);
    static bool getDebug() {return m_debug;}

private:
    static bool m_debug;
};

#endif

