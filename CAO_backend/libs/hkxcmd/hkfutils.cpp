#include "hkfutils.h"

EnumLookupType PackFlags[] = 
{
    {HKPF_XML,   "XML"},
    {HKPF_DEFAULT, "DEFAULT"},
    {HKPF_WIN32, "WIN32"},
    {HKPF_AMD64, "AMD64"},
    {HKPF_XBOX,  "XBOX"},
    {HKPF_XBOX360, "XBOX360"},
    {HKPF_TAGFILE, "TAGFILE"},
    {HKPF_TAGXML, "TAGXML"},
    {0, nullptr}
};

hkPackfileWriter::Options GetWriteOptionsFromFormat(hkPackFormat format)
{
    hkPackfileWriter::Options options;

    switch(format)
    {
    case HKPF_WIN32:
        options.m_layout = hkStructureLayout::MsvcWin32LayoutRules;
        break;
    case HKPF_AMD64:
        options.m_layout = hkStructureLayout::MsvcAmd64LayoutRules;
        break;
    case HKPF_XBOX:
        options.m_layout = hkStructureLayout::MsvcXboxLayoutRules;
        break;
    case HKPF_XBOX360:
        options.m_layout = hkStructureLayout::Xbox360LayoutRules;
        break;
    default:
        options.m_layout = hkStructureLayout::MsvcWin32LayoutRules;
        break;
    }
    return options;
}


void HK_CALL errorReport(const char* msg, void* userContext)
{
    std::cout << msg << std::endl;
}


hkResource* hkSerializeUtilLoad( hkStreamReader* stream
                                 , hkSerializeUtil::ErrorDetails* detailsOut
                                 , const hkClassNameRegistry* classReg
                                 , hkSerializeUtil::LoadOptions options )
{
    try
    {
        return hkSerializeUtil::load(stream, detailsOut, classReg, options);
    }
    catch(...)
    {
        if (detailsOut == nullptr)
            detailsOut->id = hkSerializeUtil::ErrorDetails::ERRORID_LOAD_FAILED;
        return nullptr;
    }
}


hkResult hkSerializeUtilSave( hkPackFormat pkFormat, hkVariant &root, hkOstream &stream
                              , hkSerializeUtil::SaveOptionBits flags
                              , const hkPackfileWriter::Options& packFileOptions )
{
    hkResult res;
    try
    {
        if (pkFormat == HKPF_TAGXML || pkFormat == HKPF_TAGFILE)
            res = hkSerializeUtil::saveTagfile(root.m_object, *root.m_class, stream.getStreamWriter(), nullptr, flags );
        else
            res = hkSerializeUtil::savePackfile(root.m_object, *root.m_class, stream.getStreamWriter(), packFileOptions, nullptr, flags );
    }
    catch(...)
    {
        res = HK_FAILURE;
    }
    return res;
}

hkResult LoadDefaultRegistry()
{
    hkVersionPatchManager patchManager;
    {
        extern void HK_CALL CustomRegisterPatches(hkVersionPatchManager& patchManager);
        CustomRegisterPatches(patchManager);
    }
    hkDefaultClassNameRegistry &defaultRegistry = hkDefaultClassNameRegistry::getInstance();
    {
        extern void HK_CALL CustomRegisterDefaultClasses();
        extern void HK_CALL ValidateClassSignatures();
        CustomRegisterDefaultClasses();
        ValidateClassSignatures();
    }
    return HK_SUCCESS;
}

hkResult hkSerializeLoad(hkStreamReader *reader
                         , hkVariant &root
                         , hkResource *&resource)
{
    hkTypeInfoRegistry &defaultTypeRegistry = hkTypeInfoRegistry::getInstance();
    hkDefaultClassNameRegistry &defaultRegistry = hkDefaultClassNameRegistry::getInstance();

    hkBinaryPackfileReader bpkreader;
    hkXmlPackfileReader xpkreader;
    resource = nullptr;
    hkSerializeUtil::FormatDetails formatDetails;
    hkSerializeUtil::detectFormat( reader, formatDetails );
    hkBool32 isLoadable = hkSerializeUtil::isLoadable( reader );

    if (!isLoadable && formatDetails.m_formatType != hkSerializeUtil::FORMAT_TAGFILE_XML)
        return HK_FAILURE;
    else
    {
        switch ( formatDetails.m_formatType )
        {
        case hkSerializeUtil::FORMAT_PACKFILE_BINARY:
        {
            bpkreader.loadEntireFile(reader);
            bpkreader.finishLoadedObjects(defaultTypeRegistry);
            if ( hkPackfileData* pkdata = bpkreader.getPackfileData() )
            {
                hkArray<hkVariant>& obj = bpkreader.getLoadedObjects();
                for ( int i =0,n=obj.getSize(); i<n; ++i)
                {
                    hkVariant& value = obj[i];
                    if ( value.m_class->hasVtable() )
                        defaultTypeRegistry.finishLoadedObject(value.m_object, value.m_class->getName());
                }
                resource = pkdata;
                resource->addReference();
            }
            root = bpkreader.getTopLevelObject();
        }
            break;

        case hkSerializeUtil::FORMAT_PACKFILE_XML:
        {
            xpkreader.loadEntireFileWithRegistry(reader, &defaultRegistry);
            if ( hkPackfileData* pkdata = xpkreader.getPackfileData() )
            {
                hkArray<hkVariant>& obj = xpkreader.getLoadedObjects();
                for ( int i =0,n=obj.getSize(); i<n; ++i)
                {
                    hkVariant& value = obj[i];
                    if ( value.m_class->hasVtable() )
                        defaultTypeRegistry.finishLoadedObject(value.m_object, value.m_class->getName());
                }
                resource = pkdata;
                resource->addReference();
                root = xpkreader.getTopLevelObject();
            }
        }
            break;

        case hkSerializeUtil::FORMAT_TAGFILE_BINARY:
        case hkSerializeUtil::FORMAT_TAGFILE_XML:
        default:
        {
            hkSerializeUtil::ErrorDetails detailsOut;
            hkSerializeUtil::LoadOptions loadflags = hkSerializeUtil::LOAD_FAIL_IF_VERSIONING;
            resource = hkSerializeUtilLoad(reader, &detailsOut, &defaultRegistry, loadflags);
            root.m_object = resource->getContents<hkRootLevelContainer>();
            if (root.m_object != nullptr)
                root.m_class = &(static_cast<hkRootLevelContainer*>(root.m_object))->staticClass();
        }
            break;
        }
    }
    return root.m_object != nullptr ? HK_SUCCESS : HK_FAILURE;
}

hkResource *hkSerializeLoadResource(hkStreamReader *reader)
{
    hkResource *resource = nullptr;
    hkVariant root;
    hkSerializeLoad(reader, root, resource);
    return resource;
}
