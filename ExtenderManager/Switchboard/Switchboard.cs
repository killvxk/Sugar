using System;
using System.Collections.Generic;
using System.Text;

namespace Switchboard
{
    // Extenders must have a class derived from ExtenderRegistration in order to get loaded/initialized
    public class ExtenderRegistration
    {
        public enum Error { OK, Fail, Retry };

        public virtual Error GetDependencies(out String[] depending, out String[] publishing)
        { depending = null; publishing = null; return Error.Fail; }

        public virtual Error Init()
        { return Error.OK; }

        public virtual Error PostInit()
        { return Error.OK; }

        public virtual Error ShutDown()
        { return Error.OK; }
    }
}
