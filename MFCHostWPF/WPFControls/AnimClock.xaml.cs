using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace WPFControls
{
    /// <summary>
    /// Interaction logic for AnimClock.xaml
    /// </summary>
    public partial class AnimClock : Page
    {
        private DispatcherTimer _dayTimer;

        public AnimClock()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(Clock_Loaded);
        }

        public void ChangeDateTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec)
        {
            DataContext = nDay.ToString();

            Storyboard sb = (Storyboard)PodClock.FindResource("sb");
            sb.Begin(PodClock, HandoffBehavior.SnapshotAndReplace, true);
            DateTime dtChange = new DateTime(nYear, nMonth, nDay, nHour, nMin, nSec);
            sb.Seek(PodClock, dtChange.TimeOfDay, TimeSeekOrigin.BeginTime);
        }

        void Clock_Loaded(object sender, RoutedEventArgs e)
        {          
            // then set up a timer to fire at the start of tomorrow, so that we can updateDate the datacontext
            DateTime now = DateTime.Now;
            _dayTimer = new DispatcherTimer();
            _dayTimer.Interval = new TimeSpan(1, 0, 0, 0) - now.TimeOfDay;
            _dayTimer.Tick += new EventHandler(OnDayChange);
            _dayTimer.Start();
        }

        private void OnDayChange(object sender, EventArgs e)
        {
            // date has changed, update the datacontext to reflect today's date
            DateTime now = DateTime.Now;
            DataContext = now.Day.ToString();
            _dayTimer.Interval = new TimeSpan(1, 0, 0, 0);
        }
    }
}
