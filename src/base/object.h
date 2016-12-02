// object.h

#ifndef OBJECT_H

#define OBJECT_H

#include <QObject>

/* Some helper classes for all QObjects. Esp., a simple name for debugging.
 */

extern QString objName(QObject const *);
/*:F objName
 *:D Returns either the object's own name, or the name of its class combined
     with a hex representation of its address.
*/

#endif
