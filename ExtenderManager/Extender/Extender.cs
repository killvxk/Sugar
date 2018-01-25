using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Switchboard;

namespace Extender
{
    public class ExtenderObject : ExtenderRegistration
    {
        public override Error GetDependencies(out String[] depending, out String[] publishing)
        {
            depending = null;
            publishing = null;
            return Error.Fail;
        }

        public override Error Init()
        {
            return Error.OK;
        }

        public override Error PostInit()
        {
            return Error.OK;
        }

        public override Error ShutDown()
        {
            return Error.OK;
        }
    }
}
