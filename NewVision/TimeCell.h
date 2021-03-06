using namespace System;
using namespace System::Reflection;

using namespace System::Windows::Forms;

// Adapted From
// 'http://msdn.microsoft.com/msdnmag/issues/05/04/CuttingEdge/
// and http://dzaebel.net/TimeColumn.htm

// =========================================================================================
public ref class TimeEditingControl : public DateTimePicker, public IDataGridViewEditingControl
{
	private:
		bool valueChanged;
	public:
		DataGridView^ dataGridView;
		int rowIndex;

		TimeEditingControl()
		{
			this->Format = DateTimePickerFormat::Time;
			valueChanged = false;
		}

		// Implements the IDataGridViewEditingControl.EditingControlFormattedValue property.
		virtual property Object^ EditingControlFormattedValue
		{
			Object^ get()  
			{
				return (Object^)this->Value.ToLongTimeString();
			}
			void set(Object^ value) 
			{
				String^ newValue = (String^)value ;
				if (newValue != nullptr && !String::IsNullOrEmpty(newValue))
				{
					this->Value = DateTime::Parse(newValue);
				}
			}
		}

		// Implements the IDataGridViewEditingControl.GetEditingControlFormattedValue method.
		virtual Object^ GetEditingControlFormattedValue (DataGridViewDataErrorContexts context)
		{
			return EditingControlFormattedValue;
		}

		// Implements the IDataGridViewEditingControl.ApplyCellStyleToEditingControl method.
		virtual void ApplyCellStyleToEditingControl(DataGridViewCellStyle^ dataGridViewCellStyle)
		{
			this->Font = dataGridViewCellStyle->Font;
			this->CalendarForeColor = dataGridViewCellStyle->ForeColor;
			this->CalendarMonthBackground = dataGridViewCellStyle->BackColor;
		}

		// Implements the IDataGridViewEditingControl.EditingControlRowIndex property.
		virtual property int EditingControlRowIndex
		{
			int get() 
			{
				return rowIndex;
			}
			void set(int value)
			{
				rowIndex = value;
			}
		}

		// Implements the IDataGridViewEditingControl.EditingControlWantsInputKey method.
		virtual bool EditingControlWantsInputKey(Keys key, bool dataGridViewWantsInputKey)
		{
			// Let the DateTimePicker handle the keys listed.
			switch (key & Keys::KeyCode)
			{
				case Keys::Left:
				case Keys::Up:
				case Keys::Down:
				case Keys::Right:
				case Keys::Home:
				case Keys::End:
				case Keys::PageDown:
				case Keys::PageUp:
					return true;
				default:
					return false;
			}
		}

		// Implements the IDataGridViewEditingControl.PrepareEditingControlForEdit method.
		virtual void PrepareEditingControlForEdit(bool selectAll)
		{
			// No preparation needs to be done.
		}

		// Implements the IDataGridViewEditingControl .RepositionEditingControlOnValueChange property.
		virtual property bool RepositionEditingControlOnValueChange
		{
			bool get() 
			{
				return false;
			}
		}

		// Implements the IDataGridViewEditingControl
		// .EditingControlDataGridView property.
		virtual property DataGridView^ EditingControlDataGridView
		{
			DataGridView^ get() 
			{
				return dataGridView;
			}
			void set(DataGridView^ value) 
			{
				dataGridView = value;
			}
		}
		// Implements the IDataGridViewEditingControl
		// .EditingControlValueChanged property.
		virtual property bool EditingControlValueChanged
		{
			bool get() 
			{
				return valueChanged;
			}
			void set(bool value) 
			{
				valueChanged = value;
			}
		}

		// Implements the IDataGridViewEditingControl
		// .EditingPanelCursor property.
		virtual property System::Windows::Forms::Cursor^ EditingPanelCursor
		{
			System::Windows::Forms::Cursor^ get() 
			{
				return __super::Cursor;
			}
		}

	protected:
		virtual void OnValueChanged(EventArgs^ eventargs) override
		{
			// Notify the DataGridView that the contents of the cell have changed.
			valueChanged = true;
			this->EditingControlDataGridView->NotifyCurrentCellDirty(true);
			__super::OnValueChanged(eventargs);
		}
};

// =========================================================================================
public ref class TimeCell : public DataGridViewTextBoxCell
{
	public:
		TimeCell()
	{
		DataGridViewTextBoxCell();
		// Use the short time format.
		this->Style->Format = "hh:mm:ss tt"; 
		//this->Value = DateTime::Now;
	}

	virtual void InitializeEditingControl(int rowIndex, Object^ initialFormattedValue, DataGridViewCellStyle^ dataGridViewCellStyle) override
	{
		// Set the value of the editing control to the current cell value.
		//if (String::IsNullOrEmpty((String^)initialFormattedValue))
		//	initialFormattedValue = DateTime::Now;
		__super::InitializeEditingControl(rowIndex, initialFormattedValue, dataGridViewCellStyle);
		TimeEditingControl^ ctl = (TimeEditingControl^)DataGridView->EditingControl;
		ctl->Value = DateTime::Parse((String^)this->Value);
		ctl->Format = DateTimePickerFormat::Custom;
		ctl->ShowUpDown = true;
		ctl->CustomFormat = "hh:mm:ss tt";
	}

	virtual property Type^ EditType {
		Type^ get () override 
		{
			// Return the type of the editing control that TimeCell uses.
			return TimeEditingControl::typeid;

		}
	}

	virtual property Type^ ValueType
	{
		Type^ get () override
		{
			// Return the type of the value that TimeCell contains.
			return DateTime::typeid;
		}
	}

	virtual property Object^ DefaultNewRowValue
	{
		Object^ get () override
		{
			// Use the current date and time as the default value.
			return DateTime::Now;
		}
	}
};

// =========================================================================================
public ref class TimeColumn : public DataGridViewColumn
{
	public:
		TimeColumn() 
		{
			DataGridViewColumn(gcnew TimeCell());
		}

		virtual property DataGridViewCell^ CellTemplate
		{
			DataGridViewCell^ get() override
			{
				return DataGridViewColumn::CellTemplate;
			}
			void set(DataGridViewCell^ value) override
			{
				// Ensure that the cell used for the template is a TimeCell.
				if (value != nullptr  && !value->GetType()->IsAssignableFrom(TimeCell::typeid))
				{
					throw gcnew InvalidCastException("Must be a TimeCell");
				}
				CellTemplate = value;
			}
		}
};



