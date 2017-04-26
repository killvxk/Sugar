using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace Manager
{
    class Program
    {
        static bool Callback(string command)
        {
            return true;
        }

        static void Main(string[] args)
        {
            MNative mNative = new MNative();
            Interface.CallParameter parameter = new Interface.CallParameter();
            parameter.parameter = "123";
            parameter.callback = Callback;
            IntPtr sp = IntPtr.Zero;
            sp = Marshal.AllocCoTaskMem(Marshal.SizeOf(parameter));
            Marshal.StructureToPtr(parameter, sp, false);
            mNative.helloWorld(sp);
        }
    }
}
