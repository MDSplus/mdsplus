<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="15008000">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="NI.SortType" Type="Int">3</Property>
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="Sample and Utility VIs" Type="Folder">
			<Item Name="PutGetTreeNodeExample.vi" Type="VI" URL="/&lt;vilib&gt;/MDSplus/Sample VIs/PutGetTreeNodeExample.vi"/>
			<Item Name="MakeSegmentUtility.vi" Type="VI" URL="/&lt;vilib&gt;/MDSplus/Library/Data/TreeNode/MakeSegmentUtility.vi"/>
			<Item Name="MakeSegmentExample.vi" Type="VI" URL="/&lt;vilib&gt;/MDSplus/Sample VIs/MakeSegmentExample.vi"/>
			<Item Name="WaitEventDataExamplevi.vi" Type="VI" URL="/&lt;vilib&gt;/MDSplus/Sample VIs/WaitEventDataExamplevi.vi"/>
			<Item Name="PutGetTreeNodeExpressionExample.vi" Type="VI" URL="/&lt;vilib&gt;/MDSplus/Sample VIs/PutGetTreeNodeExpressionExample.vi"/>
			<Item Name="PutRowExample.vi" Type="VI" URL="/&lt;vilib&gt;/MDSplus/Sample VIs/PutRowExample.vi"/>
			<Item Name="SetEventDataExample.vi" Type="VI" URL="/&lt;vilib&gt;/MDSplus/Sample VIs/SetEventDataExample.vi"/>
			<Item Name="Put Get over Mdsip Connection.vi" Type="VI" URL="/&lt;vilib&gt;/MDSplus/Sample VIs/Put Get over Mdsip Connection.vi"/>
		</Item>
		<Item Name="MDSplus.lvlib" Type="Library" URL="/&lt;userlib&gt;/MDSplus/Library/MDSplus.lvlib"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
			</Item>
			<Item Name="MDSobjectsLVShr.dll" Type="Document" URL="MDSobjectsLVShr.dll">
				<Property Name="NI.PreserveRelativePath" Type="Bool">true</Property>
			</Item>
			<Item Name="MDSobjectsLVShr" Type="VI" URL="MDSobjectsLVShr">
				<Property Name="NI.PreserveRelativePath" Type="Bool">true</Property>
			</Item>
		</Item>
		<Item Name="Build Specifications" Type="Build">
			<Item Name="MDSplus" Type="Source Distribution">
				<Property Name="Bld_buildCacheID" Type="Str">{9FCB7676-A0F7-49F0-B811-B5FB9314503F}</Property>
				<Property Name="Bld_buildSpecName" Type="Str">MDSplus</Property>
				<Property Name="Bld_excludedDirectory[0]" Type="Path">..</Property>
				<Property Name="Bld_excludedDirectory[0].pathType" Type="Str">relativeToCommon</Property>
				<Property Name="Bld_excludedDirectory[1]" Type="Path">instr.lib</Property>
				<Property Name="Bld_excludedDirectory[1].pathType" Type="Str">relativeToAppDir</Property>
				<Property Name="Bld_excludedDirectory[2]" Type="Path">..</Property>
				<Property Name="Bld_excludedDirectory[2].pathType" Type="Str">relativeToCommon</Property>
				<Property Name="Bld_excludedDirectory[3]" Type="Path">resource/objmgr</Property>
				<Property Name="Bld_excludedDirectory[3].pathType" Type="Str">relativeToAppDir</Property>
				<Property Name="Bld_excludedDirectory[4]" Type="Path">/C/Documents and Settings/AI_admin/My Documents/LabVIEW Data/InstCache</Property>
				<Property Name="Bld_excludedDirectoryCount" Type="Int">5</Property>
				<Property Name="Bld_localDestDir" Type="Path">../builds/NI_AB_PROJECTNAME/MDSplus</Property>
				<Property Name="Bld_localDestDirType" Type="Str">relativeToCommon</Property>
				<Property Name="Bld_previewCacheID" Type="Str">{22775D9B-01D9-4446-8A41-FCFA9BBC00A6}</Property>
				<Property Name="Bld_version.major" Type="Int">1</Property>
				<Property Name="Destination[0].destName" Type="Str">Destination Directory</Property>
				<Property Name="Destination[0].path" Type="Path">../builds/NI_AB_PROJECTNAME/MDSplus</Property>
				<Property Name="Destination[1].destName" Type="Str">Support Directory</Property>
				<Property Name="Destination[1].path" Type="Path">../builds/NI_AB_PROJECTNAME/MDSplus/data</Property>
				<Property Name="DestinationCount" Type="Int">2</Property>
				<Property Name="Source[0].itemID" Type="Str">{8A06D4B7-C408-41D0-B4C2-B64819146443}</Property>
				<Property Name="Source[0].type" Type="Str">Container</Property>
				<Property Name="Source[1].Container.applyInclusion" Type="Bool">true</Property>
				<Property Name="Source[1].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[1].itemID" Type="Ref">/My Computer/Sample and Utility VIs</Property>
				<Property Name="Source[1].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[1].type" Type="Str">Container</Property>
				<Property Name="Source[2].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[2].itemID" Type="Ref">/My Computer/MDSplus.lvlib</Property>
				<Property Name="Source[2].Library.allowMissingMembers" Type="Bool">true</Property>
				<Property Name="Source[2].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[2].type" Type="Str">Library</Property>
				<Property Name="SourceCount" Type="Int">3</Property>
			</Item>
		</Item>
	</Item>
</Project>
