using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace Interface
{
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate bool Callback([MarshalAs(UnmanagedType.LPWStr)]string command);

    [StructLayout(LayoutKind.Sequential, Pack = 8, CharSet = CharSet.Unicode)]
    public struct CallParameter
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 512)]
        public string parameter;

        public Callback callback;
    }

    public interface IManager
    {
        bool helloWorld(IntPtr parameter);
    }
}
