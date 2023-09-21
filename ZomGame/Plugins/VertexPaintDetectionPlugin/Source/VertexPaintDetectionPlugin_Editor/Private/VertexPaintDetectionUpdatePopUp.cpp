// Copyright 2022 Alexander Floden, Alias Alex River. All Rights Reserved. 

#include "VertexPaintDetectionUpdatePopUp.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "SWebBrowser.h"
#include "Interfaces/IPluginManager.h"
#include "EditorStyleSet.h"
#include "VertexPaintDetectionUpdatConfig.h"


//--------------------------------------------------------

// On Browser Link Clicked

void VertexPaintDetectionUpdatePopUp::OnBrowserLinkClicked(const FSlateHyperlinkRun::FMetadata& Metadata) {

	const FString* URL = Metadata.Find(TEXT("href"));

	if (URL)
	{
		FPlatformProcess::LaunchURL(**URL, nullptr, nullptr);
	}
}


//--------------------------------------------------------

// Register

void VertexPaintDetectionUpdatePopUp::Register() {

	
	UVertexPaintDetectionUpdatConfig* updatePopupConfig = GetMutableDefault<UVertexPaintDetectionUpdatConfig>();

	if (updatePopupConfig) {


		IPluginManager& PluginManager = IPluginManager::Get();
		auto pluginTemp = PluginManager.FindPlugin(TEXT("VertexPaintDetectionPlugin"));


		if (pluginTemp.IsValid()) {

			const FString UpdatedConfigFile = pluginTemp.Get()->GetBaseDir() + "/Config/UpdateConfig.ini";

			if (FPaths::FileExists(UpdatedConfigFile)) {

				updatePopupConfig->LoadConfig(nullptr, *UpdatedConfigFile);
			}

			else {

				updatePopupConfig->SaveConfig(CPF_Config, *UpdatedConfigFile);
			}


			const FPluginDescriptor& Descriptor = pluginTemp.Get()->GetDescriptor();

			if (Descriptor.VersionName != updatePopupConfig->PluginVersionUpdate) {

				updatePopupConfig->PluginVersionUpdate = Descriptor.VersionName;
				updatePopupConfig->SaveConfig(CPF_Config, *UpdatedConfigFile);

				FCoreDelegates::OnPostEngineInit.AddLambda([]() {
					Open();
				});
			}
		}
	}
}


//--------------------------------------------------------

// Open

void VertexPaintDetectionUpdatePopUp::Open() {

	if (!FSlateApplication::Get().CanDisplayWindows())
	{
		return;
	}

	TSharedRef<SBorder> WindowContent = SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
		.Padding(FMargin(8.0f, 8.0f));

	TSharedPtr<SWindow> Window = SNew(SWindow)
		.AutoCenter(EAutoCenter::PreferredWorkArea)
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		.SizingRule(ESizingRule::FixedSize)
		.ClientSize(FVector2D(700, 600))
		.Title(FText::FromString("Runtime Vertex Color Paint & Detection Plugin"))
		.IsTopmostWindow(true)
		[
			WindowContent
		];

	const FSlateFontInfo HeadingFont = FCoreStyle::GetDefaultFontStyle("Regular", 24);
	const FSlateFontInfo ContentFont = FCoreStyle::GetDefaultFontStyle("Regular", 12);

	TSharedRef<SVerticalBox> InnerContent = SNew(SVerticalBox)
		// Default settings example
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10)
		[
			SNew(STextBlock)
			.Font(HeadingFont)
		.Text(FText::FromString("Runtime Vertex Color Paint & Detection Plugin"))
		]
	+ SVerticalBox::Slot()
		.FillHeight(1.0)
		.Padding(10)
		[
			SNew(SBorder)
			.Padding(10)

#if ENGINE_MAJOR_VERSION == 4

		.BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))

#elif ENGINE_MAJOR_VERSION == 5

	#if ENGINE_MINOR_VERSION == 0

		.BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))

	#else

		.BorderImage(FAppStyle::GetBrush("ToolPanel.DarkGroupBorder"))

	#endif
#endif

		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
		[
			SNew(SRichTextBlock)
			.Text(FText::FromString(R"(
<LargeText>Thank you for using Runtime Vertex Color Paint & Detection Plugin!</>

If you are new to the Plugin then check out the <a id="browser" href="https://youtu.be/mLZ7fyc6kDI">Video Tutorial</> which can help you get started more quickly! The <a id="browser" href="https://docs.google.com/document/d/1bIarYezV921XkSbbZ8Pknla_4qEqKlY_NHvVwIYHGks/edit?usp=sharing">Documentation </> can also give you a better understanding of the plugin works. You can also join the <a id="browser" href="https://discord.gg/YkxMsxb4eP">Discord </> if you need any help or just want to share your work!

If you enjoy the plugin then please consider leaving a <a id="browser" href="https://www.unrealengine.com/marketplace/en-US/product/runtime-vertex-color-paint-detection-plugin">Review on the Marketplace Page</>, it would really mean a lot! <3 



<RichTextBlock.Bold>UNREAL ENGINE 4 SUPPORT IS DISCONTINUED :(</>

* Unfortunately Epic has made it so you can't Update the UE4 plugin anymore. 
  Because of this I have made the decision to move up to UE5, so the plugin will continue to be developed in the UE5 version of the Example Project, so there will be no more new Example Projects for UE4 and there is no guarantee that future releases will compile for UE4. New Components and the Editor Widget for Example will not work since they're binary files changed in UE5, so trying to use them in UE4 won't work. 
    If you want to use the latest Example Project that showcases the latest features and have the most optimimized solutions for those simple examples you will have to move up to UE5!


<LargeText>Version 1.16</>

<RichTextBlock.Bold>Example Project</>

* Added so you can Add/Remove Actors & Mesh Components in Runtime to Rain Example, additional polish and small bug fixes. 

* Added Unreal Water Blueprint and Paint Within Area Logic on it to the Example Project. 

* Big Pass at Rain Example where it supports replication where server and clients are in sync with which seeds to use for each paint loops so they paint randomly the same. Really useful if what you want to paint randomly may affect gameplay, like Acid that should damage players where clients has to see it at the exact places the server has it. 

* Pass at Auto Add Color Component where we fixed bug where if the Mesh constnatly got painted, it reset it's timer so it never for instance started drying the character in the example project if they where standing in a body of water, so the parts of the character not in water didn't get dried. Also some general improvements and clean up of logic


<RichTextBlock.Bold>New Features</>

* Added so you can choose to only randomize vertices within area of effect instead of the entire mesh, which is a much more optimized solution for things such as shower heads. 

* Added to the Material Data Asset a TMap with a Family of Physics Surface and DoesPhysicsSurfaceBelongToPhysicsSurfaceFamily() function to check if a physics surface is a part of a family, as well as editor widget implementation where you can Create a Family and Add childs. 
  This can be used if you want to Apply Colors Using Physics Surface where we can use Parents and Childs of a Family to get the Channel to Apply color on, so for instance a Wheel with Sand can Paint on a Material that may not have Sand Registered, but has a Child of it like Cobble-Sand Registered, then we can apply color to whatever channel that is on just by applying colors with Sand. The same can be done vice versa. You can also make much more comfortable implementations where you need to check if a physics surface is a part of a family, for example if electricity should be able to be spread on all Metallic Surfaces, and you have like Aluminium, Steel Crate etc. Then instead of having a bunch of branches for each Metallic one, you just run DoesPhysicsSurfaceBelongToPhysicsSurfaceFamily with Metallic, and any new Metallic like physics surface can just be registered in the Editor Widget, you don't have to jump around and update any logic in a bunch of BPs. 

* Added so you can Apply Colors Using Physics Surface, if the Material has the Physics Surface Registered it will apply colors to whatever channel it is registered to. You can also choose if a Parent Physics Surface can Apply colors on a child, and vice versa, which is quite useful if for instance a Character that has fundemental surfaces such as Sand, and is walking on Cobble-Sand or Asphalt-Sand, then if the Physics Surface Family is setup correctly where they're children of Sand, we can apply colors on whatever channel the character has Sand on. If we failed to get any colors to apply then you can optionally fall back to use whatever colors is set to the regular RGBA. 
  The Example Project has been updated so almost everything is using this now. Highly Recommend checking out the Example Projects Physics Surface Family how it's setup, and implementations of like character when we're painting his feet with whatever surface we walked on. 


<RichTextBlock.Bold>Misc</>

* Set includeAmountOfPaintedColorsOfEachChannel to be True, since it only makes tasks such as GetAllColorsOnly, Paint Color Snippet, Paint Jobs with Set instead of Adding colors, SetMeshColorsUsingColorArray to take a bit longer to finish since it will require tasks that doesn't need to loop through all vertices to do so (unless they've also been set to include vertex colors or something). 
  These are not super common, and the data from this is something we often want to use. So think it will easier for users just to set this to false if they're doing one of those tasks instead of requiring everyone to remember to turn it on.

* Removed old unnecessary parameter in Auto Add Colors Entire Mesh Component

* Added so you in Task Result can get the Associated Paint Component, as well as the Vertex Color Channels that got affected. 

* Removed so you don't have to set any Face Index for Static Mesh Components anymore since we could get the Material of the section just through code! So line traces doesn't have to be Trace Complex True etc. 

* Changed it back so we store color snippets as TArrays FColor again, since it was Much much more expensive with serialized strings. For instance a cheap mesh with 1000 verts cost 56kb, and with TArray it cost 3.9kb. If it was a 100 000 verts it would've been 5.6MB which is way to much for just 1 snippet on one mesh. 

* Did a pass at Auto Add Color Component we added last patch, where i've added a function so you can now auto paint using a physics surface, and not just on specified channel. Great if you don't know which channel for instance Wet is on! Also changed so instead of setting default values on the components we set them in the start function parameters, was more understandable since we now have paint on channel option, and with physics surface option. Also made it possible so you can Stop it, in case you want to start a new one. Added small fixes so a new one won't be started if it's already running

* Updated GetColorsToApplyFromPhysicsSurface() so it now uses the new Physics Surface Family instead of the old thing we had where we per material registered how much to return for each surface. I removed that old stuff from the editor widget since it was hard to understand etc. It ofc. also checks with whatever is registered on the channels and the blendables as well. 

* Added so in mesh vertex data per lod you get the amount of vertices as well

* Added the option IF the paint job should even be applied, so you can get things like Comparison %, amount of colors of each channel IF you apply the paint job, but without actually applying it. Also stuff like Task Duration

* Made it possible to override the seed when running Paint Entire Mesh Randomness, so you can have server and clients be synched so they all get painted randomly exactly the same. 


<RichTextBlock.Bold>Bug Fixes</>

* Fixed warning of Monolithic Headers, i.e. removed include of Engine.h and added BoxComponent

* Removed Settings from AdditionalDataToPassThrough to make it much more lightweight. It also fixed a crash issue i got with shipping. Added more floats etc. instead so whatever you used with the settings, you can simply just use the regular floats and vectors instead. 

* Fixed important bug regarding how we set the sections vertex amount, where if the previous section had more vertices, then the task failed. This could be the issue if it was a Merged Mesh. 

* Minor fix to Auto Add Colors where it doesn't add more tasks unless it doesn't have any active tasks itself. I.e. just because something else finished a paint task on the mesh, doesn't mean that it adds on itself. 

* Fixed crash issue if no material data asset was set. 

* Fixed bug in Game Instance Paint Task Finished Delegate where the amount of colors of each channel it passed through wasn't the amount After paint job has been applied, but before. 

* Fixed compile issue that could occur because of a logical ambiguous if branch




<LargeText>Version 1.15</>

<RichTextBlock.Bold>New Features</>

* 5.2 Support! 
  Unfortunately Spline Mesh Components doesn't support Vertex Painting in 5.2, even in Editor. Epic has made a Bug Tracker UE-186643 and is aware of it so hopefully it will be fixed for 5.3. 

* Added a Compare Colors Feature, where you can in the Callback settings set to compare the Current (or newly painted if paint job) Mesh Vertex Colors, with a Color Array that you pass through. Then in the task result struct you can get the result of it with a matching percent from 0-100. Useful in drawing games, tutorials etc. where you want to determine if the player has painted in the pattern you want them to. 
  Used with BP_CompareGraffitiWall in the Example Project. 

* Changed MultisphereTraceForPaintAtLocation to MultiCapsuleTraceForClosestUniqueMeshes where we Capsule Trace, which in UE5 can actually get the face indexes if set to Trace Complex! This is pretty big since it means we can get the Material if it's a static mesh components, and if the material is registered in the Editor Widget, we can use things like GetColorsToApplyFromPhysicsMaterial to get the colors to apply on RGBA. This means that you don't have to paint something with a specific RGBA, but rather get it, per material. So if one mesh has for instance Puddle on Vertex Channel Blue, and another on Red, and you run that function with Puddle, you will get the correct values to apply for the different meshes. So you're not forced to have all material be synched on what is on what channel. It can also be kind of a check, if you weren't able to get the colors, or their 0, it means the material doesn't have the physics surface registered and doesn't use the channel, so there's no need to start a paint job on the mesh. 
  I will update the Example Project to this principle where we paint using Physics Surface in the coming Weeks for UE5, with a fallbak to a specified RGBA if it's a skeletal mesh since they don't have faceIndexes and we can't get the exact material based on just a trace. So skel meshes may still have to be synched up if many should be able to get wet etc. 

  NOTE However that if you want to have Trace Complex true and be able to get face indexes, you may have to set static meshes Double Geometry to true, otherwise flat meshes like floors didn't always register hits with the capsule trace, unless they where like rotated in a certain manner. 

* Replaced the old Limit Paint Normal with new Only Affect Vertices With Direction To Actor Or Location, which makes the same functionality much more easier to use where instead of setting a World Normal, you pass through an actor, or location vector, and within what Dot the vertex normal has to have toward the location for it to be allowed to be painted. Can be used to get further control over which vertices that should get painted. For instance if you have hollow Sphere, and only want the middle vertices to be painted. This is similar to the paint condition Has Line of Sight, except this is cheaper because it doesn't run any line traces, so if you don't care about checking for blocking geometry but just direction, then this is recommended. 

* In the Example Project, added so the character can go into physics and roll around, and what they roll over get painted on them. 

* In the Example Project, added a Whiteboard, Graffiti Wall and Paper where you can walk up to thme and freely paint on them. They use a bunch of small but more expensive meshes so we can get finer detail, instead of one large extremely heavy mesh. Notice how with Graffiti, we use Has line of Sight Condition so if something is between the wall and camera then those vertices won't get painted. You can switch between Low, High and Extreme Detail. 
  Because some meshes are heavy we've set so it can use a micro delay in tick. 

* Added solution to Fill An Area between two locations with Paint with a AdjustBoxCollisionToFitAreaBetweenPaintAtLocations and Node for it, which takes in a Box Collision and adjusts its location and extent to fit between two locations, so you then can use that box collisions overlapping components and run Paint Within Area to Fill the area. Very useful if you have instances where you can do really fast motions and the distance between two paint at locations may be large, and you want to fill between them. 
  Used with for example the Whiteboard exampless where the player can drag super fast with the mouse. 


<RichTextBlock.Bold>Misc</>

* Fixed so if a Material Instance isn't registered to the data asset, it checks if it's parent is, and if so then uses whatever is registered in the parent. Useful since you don't have to register all material instances but only the parent, if you know that all the instances has the same physics surfaces etc. 

* Fixed so all functions are properly sorted into correct sub categories. 

* Changed max amount of allowed task per mesh to 50 by default, as we really don't want it to go any higher, if it does then there is something wrong with the implementation. 50 is already pretty high. 

* Added much better solution to paint the ground with the wheel component where we now use paint within area using box collision that we move in between the previous frame and current frame, with the distance as its box extent, so we always fill the gap in between. Fixes the issue with previous solution that uses paint at location, where there could be a lot of gaps in between. Check the Example Projects Vehicle for guidance. 

* Removed old GetAmountOfTaskStartdByComponent as it wasn't optimized. Replaced it with GetCurrentTasksInitiatedByComponent() where you can get the total amount, as well as the amount for each task.  

* Split up and made Check Valids much cleaner, and so SetMeshColors and SetMeshColorsSerialized uses the same check valid. Also so you for any paint job can set the canOnlyApplyPaintOnTheseActors, before it was something tied to the component so couldn't set it for SetMeshColors and SetMeshColorsSerialized. 

* Cleaned up and improved some logic in the vehicle and vehicle paint component so it uses overlaps to make it much cleaner when painting on the ground

* Added additional prints here and there for even better debugging. 

* In the Example Project, Added so drips also multi capsule trace and can paint on several meshes, which was necessary since Graffiti Wall (which is made up of several small meshes) can spawn it so it drips down. 

* Added and renamed existing falloff options so now there's spherical shape, gradiant from bottom, gradiant from top. 

* In the BPC_RuntimeVertexPaintAndDetectionComponent_VehicleWheel component, changed the default box collision names to look up to _PaintWheelCollision for the wheel and _PaintGroundCollision for the ground to make it more clearer, so remember to Update those names. You can also now set what names it should look up if you want something different or continue to use the old name.

)"))


#if ENGINE_MAJOR_VERSION == 4

.TextStyle(FEditorStyle::Get(), "NormalText")
.DecoratorStyleSet(&FEditorStyle::Get())

#elif ENGINE_MAJOR_VERSION == 5

	#if ENGINE_MINOR_VERSION == 0

.TextStyle(FEditorStyle::Get(), "NormalText")
.DecoratorStyleSet(&FEditorStyle::Get())

	#endif

.TextStyle(FAppStyle::Get(), "NormalText")
.DecoratorStyleSet(&FAppStyle::Get())

#endif


.AutoWrapText(true)
+ SRichTextBlock::HyperlinkDecorator(TEXT("browser"), FSlateHyperlinkRun::FOnClick::CreateStatic(&OnBrowserLinkClicked))
		]
		]
		]
	+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1.0f)
		[
			SNew(SButton)
			.Text(FText::FromString("Leave a Review"))
		.HAlign(HAlign_Center)
		.OnClicked_Lambda([]()
			{
				const FString URL = "https://www.unrealengine.com/marketplace/en-US/product/runtime-vertex-color-paint-detection-plugin";
				FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);

				return FReply::Handled();
			})
		]
	+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SSpacer)
			.Size(FVector2D(20, 10))
		]
	+ SHorizontalBox::Slot().FillWidth(1.0f)
		[
			SNew(SButton)
			.Text(FText::FromString("Close Window"))
		.HAlign(HAlign_Center)
		.OnClicked_Lambda([Window]()
			{
				Window->RequestDestroyWindow();

				return FReply::Handled();
			})
		]
		];

	WindowContent->SetContent(InnerContent);
	Window = FSlateApplication::Get().AddWindow(Window.ToSharedRef());
}
