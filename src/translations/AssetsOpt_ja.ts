<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ja_JP">
<context>
    <name>AnimationsOptimizer</name>
    <message>
        <location filename="../AnimationsOptimizer.cpp" line="16"/>
        <source>Animation successfully ported: </source>
        <translation>アニメーションのポートに成功しました: </translation>
    </message>
</context>
<context>
    <name>BsaOptimizer</name>
    <message>
        <location filename="../BsaOptimizer.cpp" line="30"/>
        <source>An error occured during the extraction. The BSA was correctly extracted, but the files were left inside a subdirectory.</source>
        <translation>展開中にエラーが発生しました。BSAは正しく展開されました。しかし、ファイルはサブディレクトリの中に残ってしまいました。</translation>
    </message>
    <message>
        <location filename="../BsaOptimizer.cpp" line="36"/>
        <source>An error occured during the extraction. Please extract it manually. The BSA was not deleted.</source>
        <translation>展開時エラーが発生しました: 手動で展開してください: このBSAはまだ削除されていません。</translation>
    </message>
    <message>
        <location filename="../BsaOptimizer.cpp" line="71"/>
        <source>Cannot pack existing loose files: a BSA already exists.</source>
        <translation>既存のルーズファイルを圧縮することはできません: BSAは既に存在します。</translation>
    </message>
    <message>
        <location filename="../BsaOptimizer.cpp" line="81"/>
        <source>BSA successfully compressed: </source>
        <translation>BSAの圧縮に成功しました: </translation>
    </message>
    <message>
        <location filename="../BsaOptimizer.cpp" line="87"/>
        <source>The BSA was not compressed: it is over 2.15gb: </source>
        <translation>このBSAは、2.15gbを超えているので圧縮されません: </translation>
    </message>
</context>
<context>
    <name>FilesystemOperations</name>
    <message>
        <location filename="../FilesystemOperations.cpp" line="25"/>
        <source>FilesToNotPack.txt not found. Animations will be packed, preventing them from being detected by FNIS and Nemesis.</source>
        <translation>FilesToNotPack.txtが見つかりません。アニメーションはFNISやNemesisから検出されないように圧縮されます。</translation>
    </message>
    <message>
        <location filename="../FilesystemOperations.cpp" line="137"/>
        <source>An error occurred while moving files. Try reducing path size (260 characters is the maximum)</source>
        <translation>ファイルの移動中にエラーが発生しました。ファイルパスの長さを減らしてみてください (最大数は260文字です)</translation>
    </message>
    <message>
        <location filename="../FilesystemOperations.cpp" line="210"/>
        <source>The filepath is more than 260 characters long. Please reduce it.</source>
        <translation>このファイルのパスは260文字を超えています。減らしてみてください。</translation>
    </message>
</context>
<context>
    <name>MainOptimizer</name>
    <message>
        <location filename="../MainOptimizer.cpp" line="21"/>
        <source>You are currently in the Skyrim directory. BSA won&apos;t be processed</source>
        <translation>現在、Skyrimのディレクトリ内にいます。BSAは処理されません</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="41"/>
        <source>Beginning...</source>
        <translation>開始...</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="51"/>
        <source>The setup function did not run as expected. Exiting the process.</source>
        <translation>セットアップの機能は期待されたどおりに実行されませんでした。プロセスを終了します。</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="65"/>
        <source>Current mod: </source>
        <translation>現在のMod: </translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="71"/>
        <source>Extracting BSA...</source>
        <translation>BSAを展開...</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="79"/>
        <source>BSA found ! Extracting...(this may take a long time, do not force close the program): </source>
        <translation>BSAが見つかりました！ 展開中です...(これには長時間がかかるかもしれません、プログラムを強制終了しないでください): </translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="86"/>
        <source>Optimizing animations, textures and meshes...</source>
        <translation>アニメーション、テクスチャとメッシュを最適化中です...</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="103"/>
        <source>Creating BSAs...</source>
        <translation>BSAを作成中...</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="129"/>
        <source>Assets optimization completed.</source>
        <translation>アセットの最適化が完了しました。</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="157"/>
        <source>Havok Tool not found. Are you sure the Creation Kit is installed ? You can also put HavokBehaviorPostProcess.exe in the resources folder</source>
        <translation>Havokツールが見つかりません. Creation Kitはインストール済みですか？HavokBehaviorPostProcess.exe をresourcesフォルダへ置いてください</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="167"/>
        <source> not found. Cancelling.</source>
        <translation> 見つかりません。キャンセル中です。</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="219"/>
        <source> would be converted to DDS</source>
        <translation> DDSへ変換されました</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="222"/>
        <source> would be ported to SSE</source>
        <translation> SSEへポートしました</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../mainwindow.ui" line="23"/>
        <source>Cathedral Assets Optimizer 4.2.1</source>
        <translation>Cathedral Assets Optimizer 4.2.1</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="26"/>
        <source>Will write which files would be affected, without actually processing them</source>
        <translation>ファイルへ実際の書き込みは行わず、過程だけを行います</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="94"/>
        <source>Run</source>
        <translation>実行</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="120"/>
        <source>One mod</source>
        <translation>Modを一つ</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="125"/>
        <source>Several mods (MO)</source>
        <translation>大量なMod (MO)</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="166"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Will list all files that would be modified, without actually processing them.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;実際にこれらを処理せず、変更されると思われるすべてのファイルをリスト化します。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="172"/>
        <source>Dry run</source>
        <translation>テスト実行</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="197"/>
        <source>Open Directory</source>
        <translation>ディレクトリを開く</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="213"/>
        <source>BSA</source>
        <translation>BSA</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="240"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New&apos;;&quot;&gt;Creates a new BSA after processing the previously extracted assets&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New&apos;;&quot;&gt;処理後に予め展開したアセットを、新たにBSAとして作成します&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="243"/>
        <source>Recreate BSA</source>
        <translation>BSAを作成</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="265"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Extracts any BSAs present in the mod folder.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Modフォルダ内にあるBSAを展開します。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="268"/>
        <source>Extract old BSA</source>
        <translation>BSAを展開</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="290"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;By default, loose assets aren&apos;t packed. This option put them into BSA.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;デフォルトでは、アセットは圧縮されません。 このオプションはそれらをBSAに取り込みます。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="293"/>
        <source>Pack existing loose assets</source>
        <translation>既存のルーズ化されたアセットをパックする</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="300"/>
        <source>Delete backups </source>
        <translation>バックアップを削除 </translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="307"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A BSA file cannot contain more than 2gb. This will create as many BSA as necessary. &lt;span style=&quot; font-weight:600;&quot;&gt;Warning&lt;/span&gt;. If you enable this option, the process will be considerably slowed down.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;BSAファイルは2gbを超えることはできません。 これで必要になった数だけBSAが作成されます。 &lt;span style=&quot; font-weight:600;&quot;&gt;警告&lt;/span&gt;。 このオプションを有効にした場合、プロセスはかなり遅くなります。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="310"/>
        <source>Split assets</source>
        <translation>アセットを分割</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="326"/>
        <source>Textures</source>
        <translation>テクスチャ</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="353"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Converts any TGA files into DDS, as SSE cannot read these. Attempts to convert and fix any textures that would crash the game, as some older formats are incompatible with SSE.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;SSEはこれらを読み込むことができないため、TGAファイルをDDSに変換します。 一部の古い形式はSSEと互換性がないため、ゲームをクラッシュさせる可能性のあるテクスチャを変換して修正しようとします。&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="356"/>
        <location filename="../mainwindow.ui" line="427"/>
        <location filename="../mainwindow.ui" line="526"/>
        <source>Necessary optimization</source>
        <translation>最低限必要な最適化</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="378"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;BC7 is the most efficient compression SSE supports, reducing VRAM usage without observable quality loss. Will also convert uncompressed textures to BC7.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;BC7はSSEでサポートされたとても有用な圧縮形式です。VRAMの使用量を減らしつつ画質の劣化をほとんど起こしません。圧縮されていないテクスチャをBC7へ変換します。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="381"/>
        <location filename="../mainwindow.ui" line="477"/>
        <source>Full optimization</source>
        <translation>完全な最適化</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="397"/>
        <source>Meshes</source>
        <translation>メッシュ</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="424"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Attempts to repair meshes which are guaranteed to crash the game. Headparts are included.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;ゲームをクラッシュさせることが確実なメッシュを修復しようとします。 これにはヘッドパーツが含まれています。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="449"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Perform necessary optimization, and also lightly optimizes typically non-crashing meshes. This may fix some visual issues, but also lowers quality. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;必要な最適化を実行します。また、通常ではクラッシュしないメッシュを軽く最適化します。 これは視覚的な問題を解決する可能性がありますが、クオリティは低下します。 &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="452"/>
        <source>Medium optimization</source>
        <translation>中程度の最適化</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="474"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Fully optimize all meshes. Only apply if standard mesh optimization ignored necessary files.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;すべてのメッシュを完全に最適化します。 一般的にメッシュの最適化が必要なファイルを無視した場合にのみ適用されます。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="496"/>
        <source>Animations</source>
        <translation>アニメーション</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="523"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Converts animations to SSE format. If an animation is already compatible, no change will be made and an error is logged.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;アニメーションをSSEの形式に変換します。 互換性があった場合は変更を行いません、作成されたかどうかはログでエラーを参照してください。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="557"/>
        <source>Tools</source>
        <translation>ツール</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="561"/>
        <source>Log verbosity</source>
        <translation>Logの量</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="576"/>
        <location filename="../Mainwindow.cpp" line="320"/>
        <source>Switch to dark theme</source>
        <translation>暗めのシーンに切り替える</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="581"/>
        <source>Dev mode (DANGEROUS)</source>
        <translation>開発者モード (危険)</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="586"/>
        <location filename="../mainwindow.ui" line="589"/>
        <source>Maximal</source>
        <translation>最大限</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="592"/>
        <source>Use it only if you want to report a bug</source>
        <translation>バグを報告したい場合にのみ使用してください</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="600"/>
        <source>High</source>
        <translation>多め</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="603"/>
        <source>Normal log with a bit more details</source>
        <translation>少しだけ詳細な通常のログ</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="611"/>
        <source>Normal</source>
        <translation>通常</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="614"/>
        <source>Normal log</source>
        <translation>通常のログ</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="619"/>
        <source>Minimal</source>
        <translation>最小限</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="622"/>
        <source>Will only show warnings and errors</source>
        <translation>警告とエラーのみを表示します</translation>
    </message>
    <message>
        <location filename="../Mainwindow.cpp" line="122"/>
        <source>Completed. Please read the log to check if any errors occurred (displayed in red).</source>
        <translation>完了しました。ログを読んでエラーが発生していないか確認してください (赤で表示)。</translation>
    </message>
    <message>
        <location filename="../Mainwindow.cpp" line="47"/>
        <source>You have selected to perform a dry run. No files will be modified, but BSAs will be extracted if that option was selected.</source>
        <translation>あなたはテスト実行を選択しました。このオプションを選んだ場合、ファイルへの変更は行いませんが、BSAの展開だけはされます。</translation>
    </message>
    <message>
        <location filename="../Mainwindow.cpp" line="61"/>
        <source>You have selected the several mods option. This process may take a very long time, especially if you process BSA. 
This process has only been tested on the Mod Organizer mods folder.</source>
        <translation>あなたは多数のModを処理するオプションを選択しました。特にBSAを処理する場合、このプロセスには非常に長い時間がかかるかもしれません。 
このプロセスはMod Organizerのmodsフォルダーでだけテストされています。</translation>
    </message>
    <message>
        <location filename="../Mainwindow.cpp" line="84"/>
        <source>Non existing path</source>
        <translation>パスが存在しません</translation>
    </message>
    <message>
        <location filename="../Mainwindow.cpp" line="84"/>
        <source>This path does not exist. Process aborted.</source>
        <translation>このパスは存在しません。 処理を中止しました。</translation>
    </message>
    <message>
        <location filename="../Mainwindow.cpp" line="314"/>
        <source>Switch to light theme</source>
        <translation>明るめのシーンに切り替える</translation>
    </message>
</context>
<context>
    <name>MeshesOptimizer</name>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="20"/>
        <source>No custom headparts file found. If you haven&apos;t created one, please ignore this message.</source>
        <translation>カスタムヘッドパーツファイルが見つかりません。まだ作成していない場合は、このメッセージを無視してください。</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="37"/>
        <source>Listing meshes...</source>
        <translation>メッシュを一覧化しています...</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="51"/>
        <source>Nifscan has not finished within 3 minutes. Skipping mesh optimization for this mod.</source>
        <translation>Nifscanは3分以内に終了しませんでした。このModのメッシュの最適化をスキップします。</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="116"/>
        <source>ListHeadparts has not finished within 3 minutes. Skipping headparts optimization for this mod.</source>
        <translation>ListHeadpartsは3分以内に終了しませんでした。このModのヘッドパーツの最適化をスキップします。</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="142"/>
        <location filename="../MeshesOptimizer.cpp" line="148"/>
        <location filename="../MeshesOptimizer.cpp" line="154"/>
        <location filename="../MeshesOptimizer.cpp" line="160"/>
        <location filename="../MeshesOptimizer.cpp" line="166"/>
        <source>Running NifOpt...</source>
        <translation>NifOptを実行中です...</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="142"/>
        <location filename="../MeshesOptimizer.cpp" line="148"/>
        <location filename="../MeshesOptimizer.cpp" line="154"/>
        <location filename="../MeshesOptimizer.cpp" line="160"/>
        <location filename="../MeshesOptimizer.cpp" line="166"/>
        <source>Processing: </source>
        <translation>処理中です: </translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="142"/>
        <source> as an headpart due to crashing meshes option</source>
        <translation> クラッシュするメッシュのためにヘッドパーツ オプションとして</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="148"/>
        <source> due to crashing meshes option</source>
        <translation> クラッシュするメッシュのオプション</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="154"/>
        <location filename="../MeshesOptimizer.cpp" line="166"/>
        <source> due to all meshes option</source>
        <translation> 他のメッシュのオプション</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="160"/>
        <source> due to other meshes option</source>
        <translation> 他のメッシュのオプション</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="177"/>
        <source> would be optimized as an headpart due to crashing meshes option</source>
        <translation> クラッシュするメッシュがオプションにより、headpartとして最適化されます</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="180"/>
        <source> would be optimized due to crashing meshes option</source>
        <translation> クラッシュするメッシュがオプションにより最適化されます</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="183"/>
        <source> would be optimized due to other meshes option</source>
        <translation> 残り全部のメッシュがオプションにより最適化されます</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="186"/>
        <source> would be optimized due to all meshes option</source>
        <translation> すべてのメッシュがオプションにより最適化されます</translation>
    </message>
</context>
<context>
    <name>PluginsOperations</name>
    <message>
        <location filename="../PluginsOperations.cpp" line="45"/>
        <source>Esp found: </source>
        <translation>Espが見つかりました: </translation>
    </message>
</context>
<context>
    <name>TexturesOptimizer</name>
    <message>
        <location filename="../TexturesOptimizer.cpp" line="20"/>
        <source>Compressing uncompressed texture: </source>
        <translation>テクスチャを圧縮中か展開中: </translation>
    </message>
    <message>
        <location filename="../TexturesOptimizer.cpp" line="33"/>
        <source>Converting TGA files...</source>
        <translation>TGAファイルを変換中です...</translation>
    </message>
    <message>
        <location filename="../TexturesOptimizer.cpp" line="34"/>
        <source>TGA file found: </source>
        <translation>TGAファイルが見つかりました: </translation>
    </message>
    <message>
        <location filename="../TexturesOptimizer.cpp" line="34"/>
        <source>Compressing...</source>
        <translation>圧縮中です...</translation>
    </message>
</context>
</TS>
