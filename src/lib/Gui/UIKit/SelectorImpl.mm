/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                                       |
|  Jörg Bakker (jb'at'open-multimedia.org)                                  |
|                                                                           |
|  This file is part of OMM.                                                |
|                                                                           |
|  OMM is free software: you can redistribute it and/or modify              |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  OMM is distributed in the hope that it will be useful,                   |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#import <UIKit/UIKit.h>

#include <Poco/NumberFormatter.h>
#include <vector>

#include "SelectorImpl.h"
#include "Gui/Selector.h"
#include "Gui/GuiLogger.h"
#include "ImageImpl.h"


@interface OmmGuiPicker : UIPickerView<UIPickerViewDelegate, UIPickerViewDataSource>
{
    Omm::Gui::SelectorViewImpl* _pSelectorViewImpl;
}
@end


@implementation OmmGuiPicker

- (void)setImpl:(Omm::Gui::SelectorViewImpl*)pImpl
{
    _pSelectorViewImpl = pImpl;
    self.delegate = self;
    self.dataSource = self;
}

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
    return 1;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
    return _pSelectorViewImpl->_items.size();
}


- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
    _pSelectorViewImpl->_pickedIndex = row;
}


- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    return [NSString stringWithUTF8String:_pSelectorViewImpl->_items[row].c_str()];
}

@end


@interface OmmGuiSelector : UIButton
{
    Omm::Gui::SelectorViewImpl* _pSelectorViewImpl;
    OmmGuiPicker*               _pPicker;
}

@property (readwrite, retain) UIView* inputAccessoryView;
@property (readwrite, retain) UIView* inputView;

@end


@implementation OmmGuiSelector

@synthesize inputAccessoryView;
@synthesize inputView;

- (void)setImpl:(Omm::Gui::SelectorViewImpl*)pImpl
{
    _pSelectorViewImpl = pImpl;
    [self addTarget:self action:@selector(pushed) forControlEvents:UIControlEventTouchUpInside];

    _pPicker = [[OmmGuiPicker alloc] init];
//    [_pPicker sizeToFit];
//    _pPicker.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight);
    _pPicker.showsSelectionIndicator = YES;
    self.inputView = _pPicker;

    UIToolbar* keyboardDoneButtonView = [[UIToolbar alloc] init];
    keyboardDoneButtonView.barStyle = UIBarStyleBlack;
    keyboardDoneButtonView.translucent = YES;
    keyboardDoneButtonView.tintColor = nil;
    [keyboardDoneButtonView sizeToFit];
    UIBarButtonItem* doneButton = [[[UIBarButtonItem alloc] initWithTitle:@"Done"
                    style:UIBarButtonItemStyleBordered target:self
                     action:@selector(doneClicked:)] autorelease];
    UIBarButtonItem* cancelButton = [[[UIBarButtonItem alloc] initWithTitle:@"Cancel"
                    style:UIBarButtonItemStyleBordered target:self
                     action:@selector(cancelClicked:)] autorelease];
    [keyboardDoneButtonView setItems:[NSArray arrayWithObjects:doneButton, cancelButton, nil]];
    self.inputAccessoryView = keyboardDoneButtonView;

    [_pPicker setImpl:pImpl];
}


- (BOOL)canBecomeFirstResponder
{
    return YES;
}


- (void)doneClicked:(id)sender
{
    _pSelectorViewImpl->selected();
    [self updateLabelToCurrentIndex];
    [self resignFirstResponder];
}


- (void)cancelClicked:(id)sender
{
    [self resignFirstResponder];
}


- (void)pushed
{
    if (![self becomeFirstResponder]) {
        Omm::Gui::LOG(gui, warning, "selector button cannot become first responder");
    }
}


- (void)reload
{
    [self updateLabelToCurrentIndex];
    [_pPicker reloadAllComponents];
}


- (void)updateLabelToCurrentIndex
{
    if (_pSelectorViewImpl->_items.size()) {
        [self setTitle:[NSString stringWithUTF8String:_pSelectorViewImpl->_items[_pSelectorViewImpl->_currentIndex].c_str()] forState:UIControlStateNormal];
    }
}


@end


namespace Omm {
namespace Gui {


SelectorViewImpl::SelectorViewImpl(View* pView) :
_currentIndex(0),
_pickedIndex(0)
{
    OmmGuiSelector* pNativeView = [[OmmGuiSelector alloc] init];

    [pNativeView setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    [pNativeView setImpl:this];

    initViewImpl(pView, pNativeView);
}


int
SelectorViewImpl::getCurrentIndex()
{
    return _currentIndex;
}


void
SelectorViewImpl::clear()
{
    _items.clear();
    [static_cast<OmmGuiSelector*>(_pNativeView) reload];
}


void
SelectorViewImpl::addItem(const std::string& label, Image* pImage)
{
    _items.push_back(label);
    [static_cast<OmmGuiSelector*>(_pNativeView) reload];
}


void
SelectorViewImpl::selected()
{
    _currentIndex = _pickedIndex;
    LOG(gui, debug, "selector view impl, calling selected virtual method");
    IMPL_NOTIFY_CONTROLLER(SelectorController, selected, _currentIndex);
}

}  // namespace Omm
}  // namespace Gui
