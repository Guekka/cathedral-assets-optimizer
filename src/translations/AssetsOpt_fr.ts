<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="fr_FR">
<context>
    <name>AnimationsOptimizer</name>
    <message>
        <location filename="../AnimationsOptimizer.cpp" line="16"/>
        <source>Animation successfully ported: </source>
        <translation>Animation portée avec succès: </translation>
    </message>
</context>
<context>
    <name>BsaOptimizer</name>
    <message>
        <location filename="../BsaOptimizer.cpp" line="31"/>
        <source>An error occured during the extraction. The BSA was correctly extracted, but the files were left inside a subdirectory.</source>
        <translation>Une erreur a eu lieu pendant l&apos;extraction. Le BSA a été correctement extrait, mais les fichiers ont été laissés dans un sous-dossier.</translation>
    </message>
    <message>
        <location filename="../BsaOptimizer.cpp" line="36"/>
        <source>BSA successfully extracted: </source>
        <translation>BSA extrait avec succès: </translation>
    </message>
    <message>
        <location filename="../BsaOptimizer.cpp" line="39"/>
        <source>An error occured during the extraction. Please extract it manually. The BSA was not deleted.</source>
        <translation>Une erreur a eu lieu pendant l&apos;extraction. Veuillez extraire le BSA manuellement. Le BSA n&apos;a pas été supprimé.</translation>
    </message>
    <message>
        <location filename="../BsaOptimizer.cpp" line="74"/>
        <source>Cannot pack existing loose files: a BSA already exists.</source>
        <translation>Impossible de compresser les fichiers libres existants: un BSA existe déjà.</translation>
    </message>
    <message>
        <location filename="../BsaOptimizer.cpp" line="87"/>
        <source>BSA successfully compressed: </source>
        <translation>BSA compressé avec succès: </translation>
    </message>
    <message>
        <location filename="../BsaOptimizer.cpp" line="93"/>
        <source>The BSA was not compressed: it is over 2.15gb: </source>
        <translation>Le BSA n&apos;a pas été compressé: il est supérieur à 2,15go: </translation>
    </message>
</context>
<context>
    <name>FilesystemOperations</name>
    <message>
        <location filename="../FilesystemOperations.cpp" line="25"/>
        <source>FilesToNotPack.txt not found. Animations will be packed, preventing them from being detected by FNIS and Nemesis.</source>
        <translation>Fichier &quot;FilesToNotPack.txt&quot; non trouvé. Les animations seront compressées dans le BSA, les empêchant ainsi d&apos;être détectées par FNIS et Nemesis.</translation>
    </message>
    <message>
        <location filename="../FilesystemOperations.cpp" line="138"/>
        <location filename="../FilesystemOperations.cpp" line="220"/>
        <location filename="../FilesystemOperations.cpp" line="353"/>
        <source>An error occurred while moving files. Try reducing path size (260 characters is the maximum)</source>
        <translation>Une erreur a eu lieu pendant le déplacement de fichiers. Essayez de réduire la taille du chemin (260 caractères est le maximum)</translation>
    </message>
</context>
<context>
    <name>MainOptimizer</name>
    <message>
        <location filename="../MainOptimizer.cpp" line="26"/>
        <source>You are currently in the Skyrim directory. BSA won&apos;t be processed</source>
        <translation>Vous êtes actuellement dans le dossier Skyrim. Les BSA ne seront pas traités</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="46"/>
        <source>Beginning...</source>
        <translation>Début de l&apos;opération...</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="56"/>
        <source>The setup function did not run as expected. Exiting the process.</source>
        <translation>La fonction de préparation ne s&apos;est pas déroulée comme prévu. Arrêt de l&apos;opération.</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="70"/>
        <source>Current mod: </source>
        <translation>Mod actuel: </translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="76"/>
        <source>Extracting BSA...</source>
        <translation>Extraction des BSAs...</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="85"/>
        <source>BSA found ! Extracting...(this may take a long time, do not force close the program): </source>
        <translation>BSA trouvé! Extraction...(cela pourrait prendre un long moment, ne forcez pas l&apos;arrêt du programme): </translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="179"/>
        <location filename="../MainOptimizer.cpp" line="209"/>
        <source>Optimizing animations, textures and meshes...</source>
        <translation>Optimisation des animations, des textures et des meshes...</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="109"/>
        <source>Creating BSAs...</source>
        <translation>Création des BSAs...</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="131"/>
        <source>Assets optimization completed.</source>
        <translation>Optimisation des ressources complétée.</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="159"/>
        <source>Havok Tool not found. Are you sure the Creation Kit is installed ? You can also put HavokBehaviorPostProcess.exe in the resources folder</source>
        <translation>Havok Tool non trouvé. Êtes vous sûr que le Creation Kit est installé? Vous pouvez aussi mettre HavokBehaviorPostProcess.exe dans le dossier resources</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="169"/>
        <source> not found. Cancelling.</source>
        <translation> non trouvé. Annulation.</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="179"/>
        <source>Listing meshes and fixing incompatible textures...</source>
        <translation>Détection des meshes et correction des textures incompatibles...</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="226"/>
        <source> would be compressed to BC7</source>
        <translation> serait compressé au format BC7</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="230"/>
        <source> would be converted to DDS</source>
        <translation> serait converti en DDS</translation>
    </message>
    <message>
        <location filename="../MainOptimizer.cpp" line="233"/>
        <source> would be ported to SSE</source>
        <translation> serait porté au format SSE</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../mainwindow.ui" line="26"/>
        <source>Will write which files would be affected, without actually processing them</source>
        <translation>Liste les fichiers qui seraient modifiés, sans les modifier réellement</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="94"/>
        <source>Run</source>
        <translation>Lancer</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="120"/>
        <source>One mod</source>
        <translation>Un seul mod</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="125"/>
        <source>Several mods (MO)</source>
        <translation>Plusieurs mods (MO)</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="166"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Will list all files that would be modified, without actually processing them.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Liste tous les fichiers qui seraient modifiés, sans les modifier réellement.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="172"/>
        <source>Dry run</source>
        <translation>Traitement blanc</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="197"/>
        <source>Open Directory</source>
        <translation>Ouvrir le répertoire</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="213"/>
        <source>BSA</source>
        <translation>BSA</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="268"/>
        <source>Extract old BSA</source>
        <translation>Extraction de BSAs</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="290"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;By default, loose assets aren&apos;t packed. This option put them into BSA.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Par défaut, les fichiers loose ne sont pas compressés. Cette option les met dans des BSAs.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="293"/>
        <source>Pack existing loose assets</source>
        <translation>Compression des fichiers libres existants</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="306"/>
        <source>Delete backups </source>
        <translation>Suppression des backups </translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="23"/>
        <source>Cathedral Assets Optimizer 4.2.5</source>
        <translation>Cathedral Assets Optimizer 4.2.5</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="240"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New&apos;;&quot;&gt;Creates a new BSA after processing the previously extracted assets. &lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Warning&lt;/span&gt;. If you enable this option, the process will be considerably slowed down.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-family:&apos;Courier New&apos;;&quot;&gt;Crées un nouveau BSA après avoir traité les ressources précédemment extraites. &lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Attention&lt;/span&gt;. Si vous activez cette option, le processus sera considérablement ralenti.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="243"/>
        <source>Create BSA</source>
        <translation>Création de BSAs</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="265"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Extracts any BSAs present in the mod folder. &lt;span style=&quot; font-weight:600;&quot;&gt;Warning&lt;/span&gt;. If you enable this option, the process will be considerably slowed down.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Extrait tous les BSAs présents dans le dossier du mod. &lt;span style=&quot; font-weight:600;&quot;&gt;Attention&lt;/span&gt;.Si vous activez cette option, le processus sera considérablement ralenti.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="303"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;By default, a backup of existing bsa is created. Enabling this option will disable those backups.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Par défaut, un backup des BSAs existants est créé. Activer cette option désactivera ces backups.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="316"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A BSA file cannot contain more than 2gb. This will create as many BSA as necessary. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Un BSA ne peut être supérieur à 2go. Cette option créera autant de BSAs que nécessaire. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="319"/>
        <source>Split assets</source>
        <translation>Répartition des ressources</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="335"/>
        <source>Textures</source>
        <translation>Textures</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="362"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Converts any TGA files into DDS, as SSE cannot read these. Attempts to convert and fix any textures that would crash the game, as some older formats are incompatible with SSE.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Convertit tous les fichiers TGA en DDS, car SSE ne peut les lire. Essaye de convertir et de corriger les textures qui feraient crasher le jeu.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="365"/>
        <location filename="../mainwindow.ui" line="436"/>
        <location filename="../mainwindow.ui" line="538"/>
        <source>Necessary optimization</source>
        <translation>Optimisation nécessaire</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="387"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;BC7 is the most efficient compression SSE supports, reducing VRAM usage without observable quality loss. Will also convert uncompressed textures to BC7.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;BC7 est le format de compression le plus efficace que SSE supporte, réduisant l&apos;utilisation de VRAM sans perte de qualité observable. Cette option convertira les textures non compressés en BC7.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="390"/>
        <location filename="../mainwindow.ui" line="486"/>
        <source>Full optimization</source>
        <translation>Optimisation complète</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="406"/>
        <source>Meshes</source>
        <translation>Meshes</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="433"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Attempts to repair meshes which are guaranteed to crash the game. Headparts are included.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Essaye de réparer les meshes qui feront crasher le jeu. Les meshes de tête sont inclus.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="458"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Perform necessary optimization, and also lightly optimizes typically non-crashing meshes. This may fix some visual issues, but may also lower quality. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Accomplit l&apos;optimisation nécessaire, et optimise aussi les meshes qui ne feraient pas crash le jeu. Cela pourrait résoudre quelques problèmes visuels, mais pourrait aussi diminuer la qualité. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="483"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Fully optimize all meshes. Only apply if standard mesh optimization ignored necessary files. May lower visual quality.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Optimise complètement toutes les meshes. Utilisez cette option seulement si les autres ont échoué. Pourrait diminuer la qualité visuelle.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="535"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Converts animations to SSE format. If an animation is already compatible, no change will be made.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Convertit les animations au format SSE. Si une animation est déjà compatible, aucun changement ne sera fait.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="638"/>
        <source>Medium</source>
        <translation>Moyen</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="649"/>
        <source>Low</source>
        <translation>Faible</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="461"/>
        <source>Medium optimization</source>
        <translation>Optimisation intermédiaire</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="505"/>
        <source>Animations</source>
        <translation>Animations</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="575"/>
        <source>Tools</source>
        <translation>Outils</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="579"/>
        <source>Log verbosity</source>
        <translation>Verbosité du log</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="594"/>
        <location filename="../mainwindow.cpp" line="351"/>
        <source>Switch to dark theme</source>
        <translation>Passer au thème sombre</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="599"/>
        <source>Dev mode (DANGEROUS)</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="607"/>
        <source>Max</source>
        <translation>Maximum</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="610"/>
        <source>Maximal</source>
        <translation>Maximum</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="613"/>
        <source>Use it only if you want to report a bug</source>
        <translation>Utilisez le seulement si vous voulez signaler un bug</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="624"/>
        <source>High</source>
        <translation>Élevé</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="627"/>
        <source>Normal log with a bit more details</source>
        <translation>Log normal avec un peu plus de détails</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="641"/>
        <source>Normal log</source>
        <translation>Log normal</translation>
    </message>
    <message>
        <location filename="../mainwindow.ui" line="652"/>
        <source>Will only show warnings and errors</source>
        <translation>Montrera seulement les avertissements et les erreurs</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="119"/>
        <source>Completed. Please read the log to check if any errors occurred (displayed in red).</source>
        <translation>Complété. Veuillez lire le log pour vérifier qu&apos;aucune erreur n&apos;a a eu lieu (affichées en rouge).</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="55"/>
        <source>You have selected the several mods option. This process may take a very long time, especially if you process BSA. 
This process has only been tested on the Mod Organizer mods folder.</source>
        <translation>Vous avez sélectionné l&apos;option &quot;plusieurs mods&quot;. Ce processsus pourrait prendre un temps très long, particulièrement si vous traiter les BSA.
Ce processus a seulement été testé sur le dossier mods de Mod Organizer.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="75"/>
        <source>Non existing path</source>
        <translation>Chemin non existant</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="75"/>
        <source>This path does not exist or is shorter than 5 characters. Process aborted.</source>
        <translation>Ce chemin n&apos;existe pas ou est inférieur à 5 caractères. Processus annulé.</translation>
    </message>
    <message>
        <location filename="../mainwindow.cpp" line="346"/>
        <source>Switch to light theme</source>
        <translation>Passer au thème lumineux</translation>
    </message>
</context>
<context>
    <name>MeshesOptimizer</name>
    <message>
        <source>No custom headparts file found. If you haven&apos;t created one, please ignore this message.</source>
        <translation type="vanished">Fichier &quot;customHeadparts.txt&quot; non trouvé. Cela pourrait causer des erreurs lors de la détaction des meshes de tête. Veuillez réinstaller le logiciel et, si cela ne suffit pas, contactez le développeur.</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="20"/>
        <source>No custom headparts file found. This could cause issues with headparts detection. Please reinstall the program and, if this error still appears, contact the developper.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="36"/>
        <source>Listing meshes...</source>
        <translation>Détection des meshes...</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="50"/>
        <source>Nifscan has not finished within 3 minutes. Skipping mesh optimization for this mod.</source>
        <translation>Nifscan n&apos;a pas terminé après trois minutes d&apos;attente. L&apos;optimisation des meshes pour ce mod sera passée.</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="114"/>
        <source>ListHeadparts has not finished within 3 minutes. Skipping headparts optimization for this mod.</source>
        <translation>La détection des meshes de tête ne s&apos;est pas terminée après trois minutes. L&apos;optimisation des meshes de tête sera passée pour ce mod.</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="140"/>
        <location filename="../MeshesOptimizer.cpp" line="146"/>
        <location filename="../MeshesOptimizer.cpp" line="152"/>
        <location filename="../MeshesOptimizer.cpp" line="158"/>
        <location filename="../MeshesOptimizer.cpp" line="164"/>
        <source>Running NifOpt...</source>
        <translation>Lancement de NifOpt...</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="140"/>
        <location filename="../MeshesOptimizer.cpp" line="146"/>
        <location filename="../MeshesOptimizer.cpp" line="152"/>
        <location filename="../MeshesOptimizer.cpp" line="158"/>
        <location filename="../MeshesOptimizer.cpp" line="164"/>
        <source>Processing: </source>
        <translation>Traitement en cours: </translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="140"/>
        <source> as an headpart due to crashing meshes option</source>
        <translation> en tant que mesh de tête à cause de l&apos;option &quot;Meshes provoquant des crashs&quot;</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="146"/>
        <source> due to crashing meshes option</source>
        <translation> à cause de l&apos;option &quot;Meshes provoquant des crashs&quot;</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="152"/>
        <location filename="../MeshesOptimizer.cpp" line="164"/>
        <source> due to all meshes option</source>
        <translation> à cause de l&apos;option &quot;Toutes les meshes&quot;</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="158"/>
        <source> due to other meshes option</source>
        <translation> à cause de l&apos;option &quot;Autres meshes&quot;</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="175"/>
        <source> would be optimized as an headpart due to necessary optimization</source>
        <translation> serait optimisé  en tant que mesh de tête à cause de l&apos;option &quot;Meshes provoquant des crashs&quot;</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="178"/>
        <source> would be optimized due to necessary optimization</source>
        <translation> serait optimisé  à cause de l&apos;option &quot;Optimisation  nécessaire&quot;</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="181"/>
        <source> would be optimized due to medium optimization</source>
        <translation> serait optimisé  à cause de l&apos;option &quot;optimisation moyenne&quot;</translation>
    </message>
    <message>
        <location filename="../MeshesOptimizer.cpp" line="184"/>
        <source> would be optimized due to full optimization</source>
        <translation> serait optimisé  à cause de l&apos;option &quot;optimisation complète&quot;</translation>
    </message>
</context>
<context>
    <name>PluginsOperations</name>
    <message>
        <location filename="../PluginsOperations.cpp" line="48"/>
        <source>Esp found: </source>
        <translation>Esp trouvé: </translation>
    </message>
</context>
<context>
    <name>TexturesOptimizer</name>
    <message>
        <location filename="../TexturesOptimizer.cpp" line="20"/>
        <source>Compressing uncompressed texture: </source>
        <translation>Compression d&apos;une texture non compressée: </translation>
    </message>
    <message>
        <location filename="../TexturesOptimizer.cpp" line="33"/>
        <source>Converting TGA files...</source>
        <translation>Compression des fichiers TGA...</translation>
    </message>
    <message>
        <location filename="../TexturesOptimizer.cpp" line="34"/>
        <source>TGA file found: </source>
        <translation>Fichier TGA trouvé: </translation>
    </message>
    <message>
        <location filename="../TexturesOptimizer.cpp" line="34"/>
        <source>Compressing...</source>
        <translation>Compression...</translation>
    </message>
</context>
</TS>
