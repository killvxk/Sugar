using System;
using System.Reflection;
using System.IO;

namespace Switchboard
{
    public class ExtenderManager
    {
        public ExtenderRegistration CrreateExtenders(String extenderPath)
        {
            try
            {
                Assembly assembly = Assembly.Load(Path.GetFileNameWithoutExtension(extenderPath));
                Type extenderType = typeof(ExtenderRegistration);
                foreach (Type type in assembly.GetTypes())
                {
                    if (extenderType.IsAssignableFrom(type))
                    {
                        return Activator.CreateInstance(type) as ExtenderRegistration;
                    }
                }
            }
            catch(Exception e)
            {
                throw e;
            }

            return null;
        }
    }
}
