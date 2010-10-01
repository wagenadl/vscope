// daqtask.h

#ifndef DAQTASK_H
#define DAQTASK_H

#include <daq/daqbase.h>
#include <base/types.h>
#include <QObject>

// We include NIDAQmx.h just to have TaskHandle defined. Oh well.
#include <daq/dwNIDAQmx.h> 

class daqTask: public QObject {
  /*:C daqTask
   *:D This is the base class for all four data acquisition and production
       classes (AnalogIn, AnalogOut, DigitalIn, DigitalOut). Because the
       underlying NIDAQmx library groups all these notions together in its
       "Task" concept, there is a fair amount of shared code between the
       four, and most of that is concentrated here. There is no point in
       directly creating a daqTask; use the task-specific classes instead.
  */
  Q_OBJECT;
 public:
  daqTask(QString id) throw(daqException);
  /*:F daqTask()
   *:D This is a bare-bones constructor; no resources are claimed at this time.
  */
  virtual ~daqTask();
  /*:F ~daqTask()
   *:D This is a bare-bones destructor; there are no resources to be freed
       at this time.
   *:N A severe warning is issued if a daqTask is destructed
       while still in the committed state, because that is likely to leave
       the entire NIDAQ system in an undefined and unstable state.
  */
  virtual void setTriggering(bool trg) throw(daqException);
  /*:F void setTriggering(bool trg)
   *:D This enables or disables hardware triggering of data acquisition.
       When enabled (trg=true), start() does not immediately begin acquisition,
       rather, the device waits until the PFI0 line goes high.
   *:A bool trg: true to enable, false to disable hardware triggering.
   *:N daqTask does not currently support any of NIDAQmx other triggers, 
       such as Pause or Reference triggers.
   */
  virtual void commit() throw(daqException);
  /*:F void commit()
   *:D This virtual function claims the necessary hardware resources
       for the task. This is a required step before starting acquisition/
       production. Any tasks that are attached to this one (such as a
       slave DigitalIn to an AnalogIn) are also committed.
   *:N Any parameter changes applied after the call to commit()
       return daqTask to its uncommitted state.
    :  Calling commit() when the task is already committed has no effect.
    :  Descendents wishing to override this function should bracket their
       implementation with calls to preCommit() and postCommit() rather
       than calling daqTask's commit() function.
  */
  virtual void uncommit() throw(daqException);
  /*:F void uncommit()
   *:D This virtual function returns the daqTask to its uncommitted state.
       Any tasks that are attached to this one (such as a
       slave DigitalIn to an AnalogIn) are also uncommitted.   
   *:N Calling uncommit() when the task is not committed has no effect.
    :  Descendents wishing to override this function should call
       daqTask's uncommit() function at the end of their implementation.
   */
  virtual void start() throw(daqException);
  /*:F void start()
   *:D This virtual function starts the data acquisition/production.
       Any tasks that are attached to this one (such as a
       slave DigitalIn to an AnalogIn) are also started.
   *:N It will automatically call commit() if needed, but this is not a
       recommended way to use the library.
    :  Descendents wishing to override this function should bracket their
       implementation with calls to preStart() and postStart() rather
       than calling daqTask's start() function.
   */
  virtual void stop() throw(daqException);
  /*:F void stop()
   *:D This virtual function stops the data acquisition/production.
       Any tasks that are attached to this one (such as a
       slave DigitalIn to an AnalogIn) are also stopped.
   *:N stop() does not call uncommit(), even if start() called commit().
    :  I am not sure whether it is OK to call stop() before data acquisition/
       production is complete. I think it is best to use the hasCompleted()
       function or the derived classes' acquisitionEnded and productionEnded
       signals to wait until completion, and only then call stop().
    :  Descendents wishing to override this function should bracket their
       implementation with calls to preStop() and postStop() rather
       than calling daqTask's stop() function.
  */
  virtual void abort() throw(daqException);
  /*:F void abort()
   *:D This virtual function immediately aborts data acquisition/production
       and uncommits the hardware resources associated with this task.
       Any tasks that are attached to this one (such as a
       slave DigitalIn to an AnalogIn) are also aborted.
    :  Descendents wishing to override this function should bracket their
       implementation with calls to preAbort() and postAbort() rather
       than calling daqTask's abort() function.
  */
  virtual bool hasCompleted() throw(daqException);
  /*:F bool hasCompleted()
   *:D This virtual function returns true if and only if the data acquisition/
       production has finished and stop() has not yet been called.
  */
  virtual bool isRunning() throw(daqException);
  /*:F bool isRunning()
   *:D This virtual function returns true if and only if the data acquisition/
       production has been started (or has been scheduled to start pending
       a hardware trigger) but has not yet completed.
  */
  virtual int countScansSoFar() throw(daqException);
  /*:F int countScansSoFar()
   *:D This virtual function counts how many scans (i.e. multichannel samples)
       have been acquired/produced so far (since the last start()).
  */
  virtual int countScansAvailable() throw(daqException);
  /*:F int countScansAvailable()
   *:D This virtual function counts how many scans are now available for
       reading from an input task. This number will be less than
       the total number of scans acquired so far if some of them have
       already been read.
   *:N For output tasks, this function is not available and will throw
       an exception or return meaningless results.
  */
  virtual void setPollPeriod(int nscans) throw(daqException);
  /*:F void setPollPeriod(int nscans)
   *:D This virtual function determines whether and how often an input task
       generates a dataAvailable signal.
   *:A int nscans: Number of scans between signal generation. Set to zero
       to inhibit signal generation. (acquisitionEnded signals are not
       affected.)
   *:N The period may be rounded down to evenly divide DMA buffer size as
       required by the NIDAQ library.
  */
  virtual void setTimeout(float secs) throw(daqException);
  /*:F void setTimeout(float secs)
   *:D This virtual function determines how long read() calls wait for
       data to become available.
   *:A New timeout, in seconds. Set to zero to restore the default, which
       is 5 seconds plus the time expected based on acquisition/production
       frequency and number of scans.
  */
  virtual void setFrequency(double hz) throw(daqException);
  /*:F void setFrequency(double hz)
   *:D Determines the rate of acquisition/production.
   *:A double hz: New rate, in Hertz.
   *:N This information is automatically passed on to any tasks that are
       attached to this one (such as a slave DigitalIn to an AnalogIn).
  */
  double getFrequency() const { return freqhz; }
  QString deviceID() const { return dev.id(); }
 protected:
  DAQDevice &device() const { return dev; }
  void preCommit() throw(daqException);
  /*:F void preCommit()
   *:D This does the task-independent part of preparing for a commit.
   */
  void postCommit() throw(daqException);
  /*:F void postCommit()
   *:D This does the task-independent part of finalizing a commit.
   */
  void preStart() throw(daqException);
  /*:F void preStart()
   *:D This does the task-independent part of preparing to start acquisition/
       production.
   */
  void postStart() throw(daqException);
  /*:F void postStart()
   *:D This does the task-independent part of actually starting acquisition/
       production.
   */
  void preStop() throw(daqException);
  /*:F void preStop()
   *:D This does the task-independent part of actually stopping acquisition/
       production.
   */  
  void postStop() throw(daqException);
  /*:F void postStop()
   *:D This currently does nothing, but descendents should call it anyway.
   */
  void preAbort() throw(daqException);
  /*:F void preAbort()
   *:D This does the task-independent part of actually stopping acquisition/
       production.
  */
  void postAbort() throw(daqException);
  /*:F void postAbort()
   *:D This does the task-independent part of cleaning up after abort. It
       calls stop() and uncommit().
  */
 protected: // readonly for descendents except as noted.
  int pollPeriod; // in scans
  float timeout; // in seconds
  float freqhz; // acquisition/production frequency in Hertz
  bool startTrig; // whether or not hardware triggering is used for start
  bool committed; // whether or not the task is in the committed state
  TaskHandle th; // NIDAQmx task handle. Only defined when committed is true.
  int dmabufsize; // This should be set by descendents se we can keep
                  // pollPeriod legal. NIDAQ says that pollPeriod must
                  // evenly divide dmabufsize.
  bool taskIsOutput;
  DAQDevice &dev;
 public: // for internal use only!
  virtual void callbackDone(int status);
  /*:F callbackDone
   *:D This converts NIDAQmx's "Done" callback to a Qt-style signal.
   */
  virtual void callbackEvery(int nscans);
  /*:F callbackEvery
   *:D This converts NIDAQmx's "EveryN" callback to a Qt-style signal.
   */
  virtual char const *name() const;
};

#endif
