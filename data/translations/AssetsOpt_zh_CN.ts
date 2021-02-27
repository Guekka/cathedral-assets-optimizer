<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>AdvancedAnimationsModule</name>
    <message>
        <location filename="../../src/GUI/AdvancedAnimationsModule.ui" line="14"/>
        <source>Form</source>
        <translation>Form</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedAnimationsModule.ui" line="20"/>
        <source>Base</source>
        <translation>动画处理</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedAnimationsModule.ui" line="29"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Converts animations from LE to SE format&lt;/p&gt;&lt;p&gt;Note: If an animation is already converted, an error message will be logged. This is normal as the Havok SDK cannot read SSE animations &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;将动画文件转制成SE格式&lt;/p&gt;&lt;p&gt;注意：如果动画已经是SE格式，程序会在日志中写入一段错误信息。但您不用担心您的文件，因为Havok SDK不会读取您的动画文件。 &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedAnimationsModule.ui" line="32"/>
        <source>Necessary optimization</source>
        <translation>必要转制</translation>
    </message>
</context>
<context>
    <name>AdvancedBSAModule</name>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="14"/>
        <source>Form</source>
        <translation>Form</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="20"/>
        <source>BSA</source>
        <translation>BSA处理</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="29"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Extracts all BSAs&lt;/p&gt;&lt;p&gt;&lt;br/&gt;Note: loose files are not overwritten by files in BSAs&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;BSA解包&lt;/p&gt;&lt;p&gt;&lt;br/&gt;注意：松散文件不会被BSA中的文件覆盖。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="32"/>
        <source>Extract BSAs</source>
        <translation>BSA解包</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="42"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Packs all loose files into BSAs&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;将所有松散文件打包进BSA包&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="45"/>
        <source>Create BSAs</source>
        <translation>BSA打包</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="55"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Directly optimize BSA content in memory, instead of extracting, processing and repacking. This is the recommended way to optimize files in BSAs&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;直接在内存中转制BSA包文件，而不是提取，处理和重新打包。我们推荐您使用这种方式。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="58"/>
        <source>Optimize BSAs content</source>
        <translation>BSA包转制</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="74"/>
        <source>More</source>
        <translation>其它选项</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="86"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If enabled, CAO will not make BSAs loaded by the game. &lt;/p&gt;&lt;p&gt;That is to say: &lt;/p&gt;&lt;p&gt;- For games that use plugins, CAO will not create dummy plugins &lt;/p&gt;&lt;p&gt;- For games that use the ini, CAO will not edit the INI (&lt;span style=&quot; font-weight:600;&quot;&gt;Note: CAO does not support yet editing the INI. This option will have no effect on these games&lt;/span&gt;)&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;启用后，CAO会禁止BSA包被游戏加载 &lt;/p&gt;&lt;p&gt;也就是说： &lt;/p&gt;&lt;p&gt;- 对于插件型模组：CAO将不会创建合适的加载插件&lt;/p&gt;&lt;p&gt;- 对于INI型模组，CAO将不会调整INI文件(&lt;span style=&quot; font-weight:600;&quot;&gt;注意：CAO现在还不支持编辑INI文件，此选项对INI型模组没有影响。&lt;/span&gt;)&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="89"/>
        <source>Do not make BSA loaded</source>
        <translation>禁止游戏加载BSA包</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="99"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If disabled, BSAs that can be compressed will NOT be compressed.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;勾选后将不会压缩BSA包中的文件。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="102"/>
        <source>Disallow BSA compression</source>
        <translation>禁用BSA包压缩</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="112"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If enabled, incompressible files such as sounds and strings will not be merged into the main BSA. This feature is very niche and is mostly used for very large mods such as Enderal.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;启用后，不可压缩的文件将不会被打包进BSA包中，例如字符串和声音文件。该选项在某些情况下十分管用，比如 Enderal 之类的大型模组。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="115"/>
        <source>Do not merge incompressible files</source>
        <translation>选择性打包</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="125"/>
        <source>Advanced</source>
        <translation>专业选项</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="131"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum uncompressed size for files in a BSA&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;这个大小以下的纹理不会被压缩。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="144"/>
        <source>Create textures BSA</source>
        <translation>创建纹理BSA包</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="156"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum uncompressed size for files in a textures BSA&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;这个大小以下的纹理不会被压缩。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="166"/>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="199"/>
        <source>Uncompressed max size</source>
        <translation>不被压缩的最小纹理</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="176"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If enabled, textures will always be packed into their own BSA, even when they could be merged into the main BSA&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>启用后，纹理将会被强制打包进一个单独的BSA包，即使它们能合并到主BSA包中。</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="179"/>
        <source>Do not merge textures</source>
        <translation>强制打包</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.ui" line="186"/>
        <source>Separate textures</source>
        <translation>打包选项：</translation>
    </message>
</context>
<context>
    <name>AdvancedMeshesModule</name>
    <message>
        <location filename="../../src/GUI/AdvancedMeshesModule.ui" line="14"/>
        <source>Form</source>
        <translation>Form</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedMeshesModule.ui" line="26"/>
        <source>Process meshes</source>
        <translation>网格处理</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedMeshesModule.ui" line="60"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:400;&quot;&gt;Attempts to repair meshes which are guaranteed to crash the game. Headparts are included.&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:400;&quot;&gt;尝试转制会导致游戏崩溃的网格，包括头模。&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedMeshesModule.ui" line="63"/>
        <source>Recommended</source>
        <translation>推荐方案</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedMeshesModule.ui" line="88"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Fully optimize all meshes. Only use if other settings did not work. It has very low odds to succeed&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;完全转制所有网格。仅在推荐方案失效的情况下适用，该方案的成功率很低。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedMeshesModule.ui" line="91"/>
        <source>Extensive</source>
        <translation>备选方案</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedMeshesModule.ui" line="101"/>
        <source>Advanced</source>
        <translation>专业选项</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedMeshesModule.ui" line="110"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If enabled, headparts are ignored. It is recommended to enable this option when processing already converted mods.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;启用后将忽略头模。通常用于处理已转制过的模组。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/AdvancedMeshesModule.ui" line="113"/>
        <source>Ignore headparts</source>
        <translation>忽略头模</translation>
    </message>
</context>
<context>
    <name>AdvancedTexturesModule</name>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="14"/>
        <source>Form</source>
        <translation>Form</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="26"/>
        <source>Process textures</source>
        <translation>纹理处理</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="60"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Generates mipmaps for textures, improving the performance at the cost of higher disk and vram usage.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>生成纹理映射，通过提高磁盘和内存的使用来增强纹理的表现性。</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="63"/>
        <source>Generate mipmaps</source>
        <translation>生成纹理映射</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="88"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Attempts to convert and fix any textures that would crash the game.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;尝试转制并修复会导致游戏崩溃的纹理。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="91"/>
        <source>Necessary optimization</source>
        <translation>必要转制</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="110"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Compresses uncompressed textures to the configured output format. It is recommended for SSE and FO4, but may highly decrease quality for other games.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;将未压缩的纹理转制为支持的输出格式。 建议将其用于天际SE和辐射4，用于其他游戏可能会大大降低纹理质量。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="113"/>
        <source>Compress textures</source>
        <translation>纹理压缩</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="129"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Add an alpha layer to landscape textures. Required for SSE&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;生成天际SE景观纹理所需的 alpha layer 底图。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="132"/>
        <source>Add alpha to landscape textures</source>
        <translation>为景观纹理生成 alpha</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="145"/>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="267"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Resizes texture dimensions to the given sizes. Must result in texture dimensions expressed in powers of two, no less than 4x4.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;将纹理大小调整为给定大小。 必须以2的幂来表示纹理大小，不得低于4x4。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="148"/>
        <source>Resizing</source>
        <translation>调整大小</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="166"/>
        <source>Width</source>
        <translation>宽度</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="179"/>
        <source>Height</source>
        <translation>高度</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="219"/>
        <location filename="../../src/GUI/advancedTexturesModule.cpp" line="41"/>
        <source>By ratio</source>
        <translation>调整比例</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="224"/>
        <location filename="../../src/GUI/advancedTexturesModule.cpp" line="42"/>
        <source>By fixed size</source>
        <translation>调整尺寸</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="270"/>
        <source>Minimum size:</source>
        <translation>最小值</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="280"/>
        <source>Advanced</source>
        <translation>专业选项</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="289"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Will convert TGAs to a compatible DDS format if option is selected.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;启用后，则将TGA格式转制为兼容的DDS格式。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="292"/>
        <source>Force convert</source>
        <translation>强制转制：</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="299"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If enabled, textures will always be converted to the configured output format, no matter their current state.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>将纹理强制转制成输出格式</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="302"/>
        <source>Enable forced conversion</source>
        <translation>启用强制转制</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="312"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Output format for uncompressed textures, modified textures, and converted TGA files. BC7 should only be used for SSE or FO4.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;未压缩纹理，已压缩纹理和TGA转换文件的输出格式。 提示：BC7格式仅适用于SSE或FO4。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="315"/>
        <source>Output format</source>
        <translation>输出格式：</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="322"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The output format used for modified textures&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;选择纹理的输出格式&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="326"/>
        <location filename="../../src/GUI/advancedTexturesModule.cpp" line="44"/>
        <source>BC7 (BC7_UNORM)</source>
        <translation>BC7 (BC7_UNORM)</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="331"/>
        <location filename="../../src/GUI/advancedTexturesModule.cpp" line="45"/>
        <source>BC5 (BC5_UNORM)</source>
        <translation>BC5 (BC5_UNORM)</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="336"/>
        <location filename="../../src/GUI/advancedTexturesModule.cpp" line="46"/>
        <source>BC3 (BC3_UNORM)</source>
        <translation>BC3 (BC3_UNORM)</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="341"/>
        <location filename="../../src/GUI/advancedTexturesModule.cpp" line="47"/>
        <source>BC1 (BC1_UNORM)</source>
        <translation>BC1 (BC1_UNORM)</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="346"/>
        <location filename="../../src/GUI/advancedTexturesModule.cpp" line="48"/>
        <source>Uncompressed (R8G8B8A8_UNORM)</source>
        <translation>未压缩(R8G8B8A8_UNORM)</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="357"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Textures using these formats will be converted to a supported format.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;使用这些格式的纹理将会被转换成游戏支持的纹理&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="360"/>
        <source>Unwanted formats</source>
        <translation>格式转换名单</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="373"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Unwanted formats are formats that will always be converted to the configured output format&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;格式转换名单中的选定格式将会被强制转换成输出格式&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.ui" line="376"/>
        <source>Edit</source>
        <translation>编辑</translation>
    </message>
    <message>
        <location filename="../../src/GUI/advancedTexturesModule.cpp" line="140"/>
        <source>Textures (Patterns)</source>
        <translation>纹理（模式）</translation>
    </message>
</context>
<context>
    <name>CAO::AdvancedAnimationsModule</name>
    <message>
        <location filename="../../src/GUI/AdvancedAnimationsModule.cpp" line="24"/>
        <source>Animations (Patterns)</source>
        <translation>动画（模式）</translation>
    </message>
</context>
<context>
    <name>CAO::AdvancedBSAModule</name>
    <message>
        <location filename="../../src/GUI/AdvancedBSAModule.cpp" line="83"/>
        <source>BSA (General)</source>
        <translation>BSA（全局）</translation>
    </message>
</context>
<context>
    <name>CAO::AdvancedMeshesModule</name>
    <message>
        <location filename="../../src/GUI/AdvancedMeshesModule.cpp" line="72"/>
        <source>Meshes (Patterns)</source>
        <translation>网格（模式）</translation>
    </message>
</context>
<context>
    <name>CAO::IntermediateModeModule</name>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.cpp" line="20"/>
        <source>Disabled</source>
        <translation>禁用</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.cpp" line="21"/>
        <source>By ratio</source>
        <translation>调整比例</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.cpp" line="22"/>
        <source>By fixed size</source>
        <translation>调整尺寸</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.cpp" line="95"/>
        <source>Intermediate mode</source>
        <translation>标准选项</translation>
    </message>
</context>
<context>
    <name>CAO::ListDialog</name>
    <message>
        <location filename="../../src/GUI/ListDialog.cpp" line="50"/>
        <source>New item</source>
        <translation>New item</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ListDialog.cpp" line="50"/>
        <source>Name:</source>
        <translation>名称：</translation>
    </message>
</context>
<context>
    <name>CAO::PatternsManagerWindow</name>
    <message>
        <location filename="../../src/GUI/PatternsManagerWindow.cpp" line="41"/>
        <source>New Pattern</source>
        <translation>处理模式</translation>
    </message>
    <message>
        <location filename="../../src/GUI/PatternsManagerWindow.cpp" line="41"/>
        <source>Name:</source>
        <translation>名称：</translation>
    </message>
    <message>
        <location filename="../../src/GUI/PatternsManagerWindow.cpp" line="53"/>
        <source>Base pattern</source>
        <translation>选择基础模式</translation>
    </message>
    <message>
        <location filename="../../src/GUI/PatternsManagerWindow.cpp" line="54"/>
        <source>Which pattern do you want to use as a base?</source>
        <translation>您要使用哪个模式作为基础？</translation>
    </message>
    <message>
        <location filename="../../src/GUI/PatternsManagerWindow.cpp" line="75"/>
        <source>Remove Pattern</source>
        <translation>移除模式</translation>
    </message>
    <message>
        <location filename="../../src/GUI/PatternsManagerWindow.cpp" line="76"/>
        <source>Are you sure you want to remove Pattern &apos;%1&apos;? This action cannot be undone</source>
        <translation>真的要移除模式 &apos;%1&apos;？操作无法撤销</translation>
    </message>
</context>
<context>
    <name>CAO::ProgressWindow</name>
    <message>
        <location filename="../../src/GUI/ProgressWindow.cpp" line="47"/>
        <source>Done</source>
        <translation>完成</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProgressWindow.cpp" line="65"/>
        <source>Cannot open log file</source>
        <translation>无法打开日志文件</translation>
    </message>
</context>
<context>
    <name>FileTypes</name>
    <message>
        <location filename="../../src/Settings/FileTypes.cpp" line="22"/>
        <source>slBSAStandardFiles, slBSATextureFiles and slBSATextureFiles have to contain different files.
They cannot contain the same pattern</source>
        <translation>slBSAStandardFiles, slBSATextureFiles 和slBSATextureFiles 中必须包含不同的文件。
他们不能包含相同的模式。</translation>
    </message>
</context>
<context>
    <name>GeneralSettings</name>
    <message>
        <source>BSA Max size cannot be negative</source>
        <translation type="vanished">BSA最大值不能为负数</translation>
    </message>
    <message>
        <location filename="../../src/Settings/GeneralSettings.cpp" line="46"/>
        <source>BSA Max size cannot be smaller than 0.5Gb</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../src/Settings/GeneralSettings.cpp" line="49"/>
        <source>This path is shorter than 5 characters: Path: &apos;%1&apos;</source>
        <translation>该路径长度小于5。路径： &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../../src/Settings/GeneralSettings.cpp" line="54"/>
        <source>Input path does not exist. Path: &apos;%1&apos;</source>
        <translation>输入路径不存在。路径： &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../../src/Settings/GeneralSettings.cpp" line="60"/>
        <source>&apos;Several mods&apos; mode is enabled, but this path does not seem to be handled by a mod manager.
 If you are sure you want to process this folder, please create a file named &apos;%1&apos; in this folder. This is a necessary evil to ensure safety for your mods.
Path: &apos;%2&apos;</source>
        <translation>您选择的是“置入多个模组”，但这个路径似乎不是mod管理器下的模组路径。
如果您确定要转制这个文件夹内的所有模组，请在该文件夹下创建一个名为 &apos;%1&apos; 的文件。这是为了保证您模组的安全。路径： &apos;%2&apos;</translation>
    </message>
    <message>
        <location filename="../../src/Settings/GeneralSettings.cpp" line="70"/>
        <source>&apos;Single mod&apos; mode is enabled, but this path seems to be handled by a mod manager. Path: &apos;%1&apos;</source>
        <translation>您选择的是“置入一个模组，但这个路径似乎是mod管理器下的模组路径。路径：&apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../../src/Settings/GeneralSettings.cpp" line="80"/>
        <source>Output path does not exist. Path: &apos;%1&apos;</source>
        <translation>输出路径不存在。路径： &apos;%1&apos;</translation>
    </message>
</context>
<context>
    <name>IntermediateModeModule</name>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="14"/>
        <source>Form</source>
        <translation>Form</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="20"/>
        <source>BSA</source>
        <translation>BSA处理</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="29"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Extracts all BSAs&lt;/p&gt;&lt;p&gt;&lt;br/&gt;Note: loose files are not overwritten by files in BSAs&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;BSA解包&lt;/p&gt;&lt;p&gt;&lt;br/&gt;注意：松散文件不会被BSA中的文件覆盖。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="32"/>
        <source>Extract BSAs</source>
        <translation>BSA解包</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="42"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Packs all loose files into BSAs&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;将所有松散文件打包进BSA包&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="45"/>
        <source>Create BSAs</source>
        <translation>BSA打包</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="55"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Directly optimize BSA content in memory, instead of extracting, processing and repacking. This is the recommended way to optimize files in BSAs&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;直接在内存中转制BSA包文件，而不是提取，处理和重新打包。我们推荐您使用这种方式。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="58"/>
        <source>Optimize BSAs content</source>
        <translation>BSA包转制</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="68"/>
        <source>Textures</source>
        <translation>纹理</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="77"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Compresses uncompressed textures to the configured output format. It is recommended for SSE and FO4, but may highly decrease quality for other games.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;将未压缩的纹理转制为支持的输出格式。 建议将其用于天际SE和辐射4，用于其他游戏可能会大大降低纹理质量。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="80"/>
        <source>Compress textures</source>
        <translation>纹理压缩</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="90"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Generates mipmaps for textures, improving the performance at the cost of higher disk and vram usage.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>生成纹理映射，通过提高磁盘和内存的使用来增强纹理的表现性。</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="93"/>
        <source>Generate mipmaps</source>
        <translation>生成纹理映射</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="103"/>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="254"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Resizes texture dimensions to the given sizes. Must result in texture dimensions expressed in powers of two, no less than 4x4.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;将纹理尺寸调整为给定尺寸。 必须以2的幂来表示纹理尺寸，不得低于4x4。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="106"/>
        <source>Resizing</source>
        <translation>调整大小</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="159"/>
        <source>Height</source>
        <translation>高度</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="172"/>
        <source>Width</source>
        <translation>宽度</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="233"/>
        <source>Disabled</source>
        <translation>禁用</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="238"/>
        <source>By ratio</source>
        <translation>调整比例</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="243"/>
        <source>By fixed size</source>
        <translation>调整尺寸</translation>
    </message>
    <message>
        <location filename="../../src/GUI/IntermediateModeModule.ui" line="257"/>
        <source>Minimum size:</source>
        <translation>最小值</translation>
    </message>
</context>
<context>
    <name>LevelSelector</name>
    <message>
        <location filename="../../src/GUI/LevelSelector.ui" line="14"/>
        <source>Level Selector</source>
        <translation>级别选择</translation>
    </message>
    <message>
        <location filename="../../src/GUI/LevelSelector.ui" line="30"/>
        <source>Remember my choice</source>
        <translation>记住选择</translation>
    </message>
    <message>
        <location filename="../../src/GUI/LevelSelector.cpp" line="115"/>
        <source>Quick Auto Port
Quick Auto Port uses default settings for porting a mod between LE and SSE.
It will work for most of the cases and is the recommended way to port a mod.
It is safe to apply it on a mod, and it is recommended to apply it to your whole mod list if you experience crashes.</source>
        <translation>快速级
完全使用默认配置，用于快速转制模组。
大多数时候我们推荐您使用该级别。
它能安全地转制您的mod，所以我们推荐您使用该级别来转制会导致您游戏崩溃的模组</translation>
    </message>
    <message>
        <location filename="../../src/GUI/LevelSelector.cpp" line="123"/>
        <source>Intermediate mode
Intermediate mode offers default settings for most use cases, as well as being a bit more customizable than Quick Auto Port.</source>
        <translation>标准级
大部分为默认配置，但比快速级CAO的自由度更高。</translation>
    </message>
    <message>
        <location filename="../../src/GUI/LevelSelector.cpp" line="129"/>
        <source>Advanced mode
The full CAO experience. With profiles and patterns, you can fully customize how CAO will optimize your files.</source>
        <translation>专业级
开放所有CAO选项，包括配置方案和处理模式。您可以自由定义CAO对模组的转制。</translation>
    </message>
    <message>
        <location filename="../../src/GUI/LevelSelector.cpp" line="138"/>
        <source>Unknown mode</source>
        <translation>未知转制等级</translation>
    </message>
</context>
<context>
    <name>ListDialog</name>
    <message>
        <location filename="../../src/GUI/ListDialog.ui" line="14"/>
        <source>Dialog</source>
        <translation>对话框</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ListDialog.ui" line="20"/>
        <source>Search</source>
        <translation>搜索</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ListDialog.ui" line="33"/>
        <source>Add item</source>
        <translation>Add item</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ListDialog.ui" line="40"/>
        <source>Close</source>
        <translation>关闭</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="28"/>
        <source>Cathedral Assets Optimizer</source>
        <translation>Cathedral Assets Optimizer</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="31"/>
        <source>Will write which files would be affected, without actually processing them</source>
        <translation>不实际处理文件，但会指明受到影响的文件</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="53"/>
        <location filename="../../src/GUI/MainWindow.ui" line="95"/>
        <source>Manage</source>
        <translation>管理</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="69"/>
        <location filename="../../src/GUI/MainWindow.ui" line="82"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;See the wiki&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;前往维基&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="72"/>
        <source>Profile</source>
        <translation>配置方案</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="85"/>
        <source>Pattern</source>
        <translation>处理模式</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="154"/>
        <location filename="../../src/GUI/MainWindow.cpp" line="216"/>
        <location filename="../../src/GUI/MainWindow.cpp" line="257"/>
        <source>Open Directory</source>
        <translation>选择文件夹</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="179"/>
        <source>Run</source>
        <translation>运行</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="219"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Will list all files that would be modified, without actually processing them. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;勾选后CAO将不会实际处理文件，而是在日志中列出所有处理行为。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="225"/>
        <source>Dry run</source>
        <translation>测试运行</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="251"/>
        <location filename="../../src/GUI/MainWindow.cpp" line="35"/>
        <source>One mod</source>
        <translation>置入一个模组</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="256"/>
        <location filename="../../src/GUI/MainWindow.cpp" line="36"/>
        <source>Several mods</source>
        <translation>置入多个模组</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="284"/>
        <source>Options</source>
        <translation>选项</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="292"/>
        <source>Help</source>
        <translation>帮助</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="299"/>
        <source>Output</source>
        <translation>输出设置</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="306"/>
        <source>Supporting CAO</source>
        <translation>支持作者</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="312"/>
        <location filename="../../src/GUI/MainWindow.ui" line="346"/>
        <location filename="../../src/GUI/MainWindow.cpp" line="97"/>
        <source>About</source>
        <translation>关于</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="331"/>
        <source>Enable dark theme</source>
        <translation>启用暗色主题</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="336"/>
        <source>Documentation</source>
        <translation>N网页面</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="341"/>
        <source>Discord</source>
        <translation>讨论交流</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="351"/>
        <source>About Qt</source>
        <translation>关于Qt</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="356"/>
        <source>Donate</source>
        <translation>捐赠</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="361"/>
        <source>Select GPU</source>
        <translation>选择 GPU</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="369"/>
        <source>Enable output redirection</source>
        <translation>启用自定义输出路径</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="374"/>
        <source>Set output path</source>
        <translation>设置输出路径</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.ui" line="379"/>
        <source>Change level</source>
        <translation>更改转制等级</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.cpp" line="140"/>
        <source>Pattern not found</source>
        <translation>未找到该处理模式</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.cpp" line="141"/>
        <source>Couldn&apos;t find the pattern &apos;*&apos;. Please reinstall the application</source>
        <translation>找不到默认处理模式&apos;*&apos;。请尝试重新安装应用程序</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.cpp" line="342"/>
        <source>Done</source>
        <translation>完成</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.cpp" line="357"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.cpp" line="358"/>
        <source>An exception has been encountered and the process was forced to stop: </source>
        <translation>遇到异常，进程被迫停止：</translation>
    </message>
    <message>
        <location filename="../../src/GUI/MainWindow.cpp" line="397"/>
        <source>Welcome to %1 %2</source>
        <translation>欢迎使用 %1 %2</translation>
    </message>
</context>
<context>
    <name>PatternSettings</name>
    <message>
        <location filename="../../src/Settings/PatternSettings.cpp" line="72"/>
        <source>Textures resizing minimum size has to be a power of two</source>
        <translation>纹理最小尺寸必须是2的幂</translation>
    </message>
    <message>
        <location filename="../../src/Settings/PatternSettings.cpp" line="75"/>
        <source>Textures resizing target has to be a power of two</source>
        <translation>纹理尺寸必须是2的幂</translation>
    </message>
    <message>
        <location filename="../../src/Settings/PatternSettings.cpp" line="78"/>
        <source>This meshes optimization level does not exist. Level: %1</source>
        <translation>此网格转制级别不存在。级别：%1</translation>
    </message>
</context>
<context>
    <name>PatternsManagerWindow</name>
    <message>
        <location filename="../../src/GUI/PatternsManagerWindow.ui" line="14"/>
        <source>Patterns manager</source>
        <translation>处理模式管理器</translation>
    </message>
    <message>
        <location filename="../../src/GUI/PatternsManagerWindow.ui" line="20"/>
        <source>Pattern</source>
        <translation>处理模式</translation>
    </message>
    <message>
        <location filename="../../src/GUI/PatternsManagerWindow.ui" line="27"/>
        <source>New</source>
        <translation>新建</translation>
    </message>
    <message>
        <location filename="../../src/GUI/PatternsManagerWindow.ui" line="37"/>
        <source>Remove</source>
        <translation>移除</translation>
    </message>
</context>
<context>
    <name>ProfilesManagerWindow</name>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.ui" line="14"/>
        <source>Profiles manager</source>
        <translation>配置方案管理器</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.ui" line="20"/>
        <source>New</source>
        <translation>新建</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.ui" line="27"/>
        <source>Rename</source>
        <translation>重命名</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.ui" line="37"/>
        <source>Remove</source>
        <translation>移除</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.ui" line="44"/>
        <source>Profile</source>
        <translation>配置方案</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.ui" line="51"/>
        <source>Corresponding game</source>
        <translation>对应的游戏</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.ui" line="59"/>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="24"/>
        <source>Morrowind</source>
        <translation>晨风</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.ui" line="64"/>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="25"/>
        <source>Oblivion</source>
        <translation>湮没</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.ui" line="69"/>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="26"/>
        <source>Skyrim LE (2011)</source>
        <translation>天际 LE (2011)</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.ui" line="74"/>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="27"/>
        <source>Skyrim SE (2016)</source>
        <translation>天际 SE (2016)</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.ui" line="79"/>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="28"/>
        <source>Fallout 3</source>
        <translation>辐射 3</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.ui" line="84"/>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="29"/>
        <source>Fallout New Vegas</source>
        <translation>辐射：新维加斯</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.ui" line="89"/>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="30"/>
        <source>Fallout 4</source>
        <translation>辐射 4</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="86"/>
        <source>New profile</source>
        <translation>新建配置方案</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="86"/>
        <source>Name:</source>
        <translation>名称：</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="98"/>
        <source>Base profile</source>
        <translation>基础配置方案</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="99"/>
        <source>Which profile do you want to use as a base?</source>
        <translation>您要使用哪个配置方案作为基础？</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="123"/>
        <source>Remove profile</source>
        <translation>移除配置方案</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="124"/>
        <source>Are you sure you want to remove profile &apos;%1&apos;? This action cannot be undone</source>
        <translation>真的要移除配置方案&apos;%1&apos;？操作无法撤销</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="131"/>
        <source>Removal failed</source>
        <translation>删除失败</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="131"/>
        <source>Failed to delete profile: %1</source>
        <translation>无法删除配置方案：%1</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="138"/>
        <source>Rename profile</source>
        <translation>重命名</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="138"/>
        <source>New name:</source>
        <translation>新名称：</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="145"/>
        <source>Renaming failed</source>
        <translation>重命名失败</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProfilesManagerWindow.cpp" line="145"/>
        <source>Failed to rename profile: %1</source>
        <translation>无法重命名配置方案：%1</translation>
    </message>
</context>
<context>
    <name>ProgressWindow</name>
    <message>
        <location filename="../../src/GUI/ProgressWindow.ui" line="14"/>
        <source>Progress</source>
        <translation>处理信息</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProgressWindow.ui" line="26"/>
        <source>Clear log</source>
        <translation>清除日志</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProgressWindow.ui" line="33"/>
        <source>Error</source>
        <translation>错误信息</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProgressWindow.ui" line="43"/>
        <source>Info</source>
        <translation>常规信息</translation>
    </message>
    <message>
        <location filename="../../src/GUI/ProgressWindow.ui" line="53"/>
        <source>Debug</source>
        <translation>调试信息</translation>
    </message>
</context>
<context>
    <name>SelectGPUWindow</name>
    <message>
        <location filename="../../src/GUI/SelectGPUWindow.ui" line="14"/>
        <source>Select GPU</source>
        <translation>选择 GPU</translation>
    </message>
    <message>
        <location filename="../../src/GUI/SelectGPUWindow.ui" line="20"/>
        <source>GPU</source>
        <translation>GPU</translation>
    </message>
</context>
</TS>
