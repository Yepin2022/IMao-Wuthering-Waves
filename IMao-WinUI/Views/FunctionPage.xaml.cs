using IMao_WinUI.ViewModels;

using Microsoft.UI.Xaml.Controls;

namespace IMao_WinUI.Views;

public sealed partial class FunctionPage : Page
{
    public FunctionViewModel ViewModel
    {
        get;
    }

    public FunctionPage()
    {
        ViewModel = App.GetService<FunctionViewModel>();
        InitializeComponent();
    }

    private void UpdateMinMapItemDataCycle_ValueChanged(object sender, Microsoft.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
    {
        IMaoCoreAPI.SetMinMapDataUpdateCycle((int)e.NewValue);
    }

    private void UpdateMapItemDataCycle_ValueChanged(object sender, Microsoft.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
    {
        IMaoCoreAPI.SetMapDataUpdateCycle((int)e.NewValue);
    }

    private void ToggleSwitch_MapShowItem(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
    {
        ToggleSwitch toggleSwitch = sender as ToggleSwitch;
        if (toggleSwitch != null)
        {
            if (toggleSwitch.IsOn == true)
            { 
                IMaoCoreAPI.EnabledMapShowItem(true);
            }
            else
            {
                IMaoCoreAPI.EnabledMapShowItem(false);
            }
        }
    }

    private void ToggleSwitch_MinMapShowItem(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
    {
        ToggleSwitch toggleSwitch = sender as ToggleSwitch;
        if (toggleSwitch != null)
        {
            if (toggleSwitch.IsOn == true)
            {
                IMaoCoreAPI.EnabledMinMapShowItem(true);
            }
            else
            {
                IMaoCoreAPI.EnabledMinMapShowItem(false);
            }
        }
    }

    private void ToggleSwitch_SetVisibleSavedPoints(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
    {
        ToggleSwitch toggleSwitch = sender as ToggleSwitch;
        if (toggleSwitch != null)
        {
            if (toggleSwitch.IsOn == true)
            {
                IMaoCoreAPI.SetVisibleSavedPoints(true);
            }
            else
            {
                IMaoCoreAPI.SetVisibleSavedPoints(false);
            }
        }
    }
}
