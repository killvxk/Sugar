using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Switchboard;
using Extender;

namespace Test
{
    class Program
    {
        static void Main(string[] args)
        {
            ExtenderManager manager = new ExtenderManager();
            ExtenderRegistration extender = manager.CrreateExtenders("Extender.dll");
            extender.Init();
        }
    }
}
