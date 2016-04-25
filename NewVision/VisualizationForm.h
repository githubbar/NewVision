#if !defined( VISUALIZATOINFORM_H )
#define VISUALIZATOINFORM_H

#pragma once
#include "NewVisionDoc.h"
#include "vtkGlyph2D.h"
#include "vtkImageData.h"
#include "vtkPolyDataMapper.h"
//#include "vtkMPIController.h"
#include "vtkRTAnalyticSource.h"
#include "vtkFieldDataToAttributeDataFilter.h"
#include "vtkAttributeDataToFieldDataFilter.h"
#include "vtkImageShrink3D.h"
#include "vtkGlyph3D.h"
#include "vtkGlyphSource2D.h"
#include "vtkImageGradient.h"
#include "vtkImageGradientMagnitude.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkProbeFilter.h"
#include "vtkDataSetMapper.h"
#include "vtkContourFilter.h"
#include "vtkActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkDataSetAttributes.h"
#include "vtkAssignAttribute.h"
//#include "vtkFormsWindowControl.h"
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace vtk;


namespace NewVision {

	/// <summary>
	/// Summary for VisualizationForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class VisualizationForm : public System::Windows::Forms::Form
	{
	private: 
		vtk::vtkFormsWindowControl^  vtkFormsWindowControl1;
		vtk::vtkBoxWidget ^m_boxWidget;
		CNewVisionDoc *doc;
	public: 
		VisualizationForm(CNewVisionDoc *doc)
		{

			InitializeComponent();

			// Initialize VTK and display data
			this->doc = doc;
			//this->AddConeToWindow(this->vtkFormsWindowControl1->GetRenderWindow());
			//this->AddGlyphsFromData(this->vtkFormsWindowControl1->GetRenderWindow());
			this->Show(this->ProduceVectorField(this->vtkFormsWindowControl1->GetRenderWindow()));
		
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~VisualizationForm()
		{
			if (components)
			{
				delete components;
			}
		}
	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
//--------------------------------------------------------------------------------------------------
	private: vtk::vtkPolyDataMapper^ ProduceVectorField(vtk::vtkRenderWindow ^renWin) {  
		//double data = 5.157;
		//double extent = data;
		//int iextent = static_cast<int>(data);  
		//// The pipeline

		//// Synthetic image source.
		//vtk::vtkRTAnalyticSource^ source1 = gcnew vtk::vtkRTAnalyticSource();
		//source1->SetWholeExtent (-1*iextent, iextent, -1*iextent, iextent, 
		//	-1*iextent, iextent );
		//source1->SetCenter(0, 0, 0);
		//source1->SetStandardDeviation( 0.5 );
		//source1->SetMaximum( 255.0 );
		//source1->SetXFreq( 60 );
		//source1->SetXMag( 10 );
		//source1->SetYFreq( 30 );
		//source1->SetYMag( 18 );
		//source1->SetZFreq( 40 );
		//source1->SetZMag( 5 );
		//source1->GetOutput()->SetSpacing(2.0/extent,2.0/extent,2.0/extent);

		//// Gradient vector.
		//vtk::vtkImageGradient^ grad = gcnew vtk::vtkImageGradient();
		//grad->SetDimensionality( 3 );
		//grad->SetInputConnection(source1->GetOutputPort());

		//vtk::vtkImageShrink3D^ mask = gcnew vtk::vtkImageShrink3D();
		//mask->SetInputConnection(grad->GetOutputPort());
		//mask->SetShrinkFactors(5, 5, 5);


		//// Label the scalar field as the active vectors.
		//vtk::vtkAssignAttribute^ aa = gcnew vtk::vtkAssignAttribute();
		//aa->SetInputConnection(mask->GetOutputPort());
		//aa->Assign(vtk::vtkDataSetAttributes::SCALARS, vtk::vtkDataSetAttributes::VECTORS,
		//	vtk::vtkAssignAttribute::POINT_DATA);

		//vtk::vtkGlyphSource2D^ arrow = gcnew vtk::vtkGlyphSource2D();
		//arrow->SetGlyphTypeToArrow();
		//arrow->SetScale(0.2);
		//arrow->FilledOff();

		//// Glyph the gradient vector (with arrows)
		//vtk::vtkGlyph3D^ glyph = gcnew vtk::vtkGlyph3D();
		//glyph->SetInputConnection(aa->GetOutputPort());
		//glyph->SetSource(arrow->GetOutput());
		//glyph->ScalingOff();
		//glyph->OrientOn();
		//glyph->SetVectorModeToUseVector();
		//glyph->SetColorModeToColorByVector();

		//// Rendering objects.
		vtk::vtkPolyDataMapper^ mapper = gcnew vtk::vtkPolyDataMapper();
		//mapper->SetInputConnection(glyph->GetOutputPort());
		//mapper->SetScalarRange(50, 180);
		//mapper->ImmediateModeRenderingOn();

		//vtk::vtkActor^ actor = gcnew vtk::vtkActor();
		//actor->SetMapper(mapper);

		//vtk::vtkRenderer^ ren = gcnew vtk::vtkRenderer();
		//renWin->AddRenderer(ren);

		//ren->AddActor(actor);
		//ren->SetActiveCamera( cam );

		//// Cleanup
		//source1->Delete();
		//grad->Delete();
		//aa->Delete();
		//mask->Delete();
		//glyph->Delete();
		//arrow->Delete();
		//actor->Delete();
		//ren->Delete();

		return mapper;
	}
 //--------------------------------------------------------------------------------------------------
	private: void Show(vtk::vtkPolyDataMapper^ mapper)
	{
		//vtk::vtkActor ^actor = gcnew vtk::vtkActor();
		//actor->SetMapper(mapper);

		//vtk::vtkRenderer ^ren1 = gcnew vtk::vtkRenderer();
		//ren1->AddActor(actor);
		//ren1->SetBackground(0.1f, 0.2f, 0.4f);

		//renWin->AddRenderer(ren1);

		//vtk::vtkRenderWindowInteractor ^iren = renWin->GetInteractor();

		//{
		//	m_boxWidget = gcnew vtk::vtkBoxWidget();
		//	m_boxWidget->SetInteractor(iren);
		//	m_boxWidget->SetPlaceFactor(1.25f);

		//	m_boxWidget->SetProp3D(actor);
		//	m_boxWidget->PlaceWidget();

		//	m_boxWidget->AddObserver((ULONG)vtk::EventIds::InteractionEvent, gcnew vtk::vtkDotNetCallback(myCallback));

		//	m_boxWidget->On();
		//}
	}

//--------------------------------------------------------------------------------------------------
	private: void AddGlyphsFromData(vtk::vtkRenderWindow ^renWin)
	{
		vtk::vtkGlyph3D ^glyph = gcnew vtk::vtkGlyph3D();
		vtk::vtkSphereSource ^sphere = gcnew vtk::vtkSphereSource();
		vtk::vtkConeSource ^cone = gcnew vtk::vtkConeSource();;
		vtk::vtkArrowSource ^arrow = gcnew vtk::vtkArrowSource();;

		glyph->SetInput(sphere->GetOutput());
		glyph->SetSourceConnection(arrow->GetOutputPort());
		glyph->SetVectorModeToUseVector();
		glyph->SetScaleModeToScaleByVector();
		glyph->SetScaleFactor(0.1);

		vtk::vtkPolyDataMapper ^mapper= gcnew vtk::vtkPolyDataMapper();
		mapper->SetInput(glyph->GetOutput());

		vtk::vtkActor ^actor = gcnew vtk::vtkActor();
		actor->SetMapper(mapper);

		vtk::vtkRenderer ^ren1 = gcnew vtk::vtkRenderer();
		ren1->AddActor(actor);
		ren1->SetBackground(0.1f, 0.2f, 0.4f);

		renWin->AddRenderer(ren1);

		vtk::vtkRenderWindowInteractor ^iren = renWin->GetInteractor();

		{
			m_boxWidget = gcnew vtk::vtkBoxWidget();
			m_boxWidget->SetInteractor(iren);
			m_boxWidget->SetPlaceFactor(1.25f);

			m_boxWidget->SetProp3D(actor);
			m_boxWidget->PlaceWidget();

			m_boxWidget->AddObserver((ULONG)vtk::EventIds::InteractionEvent, gcnew vtk::vtkDotNetCallback(myCallback));

			m_boxWidget->On();
		}
	}
//--------------------------------------------------------------------------------------------------
	private: void AddConeToWindow(vtk::vtkRenderWindow ^renWin)
	{
		// 
		// Next we create an instance of vtkConeSource and set some of its
		// properties. The instance of vtkConeSource "cone" is part of a visualization
		// pipeline (it is a source process object); it produces data (output type is
		// vtkPolyData) which other filters may process.
		//
		vtk::vtkConeSource ^cone = gcnew vtk::vtkConeSource();
		cone->SetHeight(3.0f);
		cone->SetRadius(1.0f);
		cone->SetResolution(10);

		// 
		// In this example we terminate the pipeline with a mapper process object.
		// (Intermediate filters such as vtkShrinkPolyData could be inserted in
		// between the source and the mapper.)  We create an instance of
		// vtkPolyDataMapper to map the polygonal data into graphics primitives. We
		// connect the output of the cone souece to the input of this mapper.
		//
		vtk::vtkPolyDataMapper ^coneMapper = gcnew vtk::vtkPolyDataMapper();
		coneMapper->SetInput(cone->GetOutput());

		// 
		// Create an actor to represent the cone. The actor orchestrates rendering of
		// the mapper's graphics primitives. An actor also refers to properties via a
		// vtkProperty instance, and includes an internal transformation matrix. We
		// set this actor's mapper to be coneMapper which we created above.
		//
		vtk::vtkActor ^coneActor = gcnew vtk::vtkActor();
		coneActor->SetMapper(coneMapper);

		//
		// Create the Renderer and assign actors to it. A renderer is like a
		// viewport. It is part or all of a window on the screen and it is
		// responsible for drawing the actors it has.  We also set the background
		// color here
		//
		vtk::vtkRenderer ^ren1 = gcnew vtk::vtkRenderer();
		ren1->AddActor(coneActor);
		ren1->SetBackground(0.1f, 0.2f, 0.4f);

		//
		// Finally we create the render window which will show up on the screen
		// We put our renderer into the render window using AddRenderer. We also
		// set the size to be 300 pixels by 300
		//
		renWin->AddRenderer(ren1);

		vtk::vtkRenderWindowInteractor ^iren = renWin->GetInteractor();

		{
			m_boxWidget = gcnew vtk::vtkBoxWidget();
			m_boxWidget->SetInteractor(iren);
			m_boxWidget->SetPlaceFactor(1.25f);

			m_boxWidget->SetProp3D(coneActor);
			m_boxWidget->PlaceWidget();

			m_boxWidget->AddObserver((ULONG)vtk::EventIds::InteractionEvent, gcnew vtk::vtkDotNetCallback(myCallback));

			m_boxWidget->On();
		}
	}
//--------------------------------------------------------------------------------------------------
	private: static void myCallback(vtk::vtkObject ^caller, ULONG eventId, System::Object^  s, System::IntPtr callData)
	 {
		 System::Diagnostics::Debug::WriteLine("Callback has been called.");
		 vtk::vtkBoxWidget ^boxWidget = vtk::vtkBoxWidget::SafeDownCast(caller);
		 if (boxWidget)
		 {
			 vtk::vtkTransform ^t = gcnew vtk::vtkTransform();
			 boxWidget->GetTransform(t);
			 boxWidget->GetProp3D()->SetUserTransform(t);
		 }
		 else
		 {
			 System::Diagnostics::Debug::WriteLine("Caller is not a box widget.");
		 }
	 }
//--------------------------------------------------------------------------------------------------
#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->vtkFormsWindowControl1 = (gcnew vtk::vtkFormsWindowControl());

			this->SuspendLayout();
			// 
			// vtkFormsWindowControl1
			// 
			this->vtkFormsWindowControl1->Location = System::Drawing::Point(12, 12);
			this->vtkFormsWindowControl1->Name = L"vtkFormsWindowControl1";
			this->vtkFormsWindowControl1->Size = System::Drawing::Size(705, 579);
			this->vtkFormsWindowControl1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->vtkFormsWindowControl1->TabIndex = 0;
			this->vtkFormsWindowControl1->Text = L"vtkFormsWindowControl1";
			this->vtkFormsWindowControl1->Click += gcnew System::EventHandler(this, &VisualizationForm::vtkFormsWindowControl1_Click);
			// 
			// VisualizationForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(729, 603);
			this->Controls->Add(this->vtkFormsWindowControl1);
			this->Name = L"VisualizationForm";
			this->Text = L"VisualizationForm";
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void vtkFormsWindowControl1_Click(System::Object^  sender, System::EventArgs^  e) {
			 }
};
}
#endif // !defined( VISUALIZATOINFORM_H )