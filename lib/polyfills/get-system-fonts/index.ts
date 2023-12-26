import * as path from 'path';
import * as os from 'os';
import recursiveWalk from './recursiveWalk';

const directories: { [K in NodeJS.Platform]?: () => string[]; } = {
  win32: () => {
    const globalDir = path.join(process.env.WINDIR || 'C:\\Windows', 'Fonts');

    const appDataDir = 'Microsoft\\Windows\\Fonts';

    let localDir: string | undefined;
    if (process.env.LOCALAPPDATA) {
      localDir = path.join(process.env.LOCALAPPDATA, appDataDir);
    } else if (process.env.APPDATA) {
      localDir = path.join(process.env.APPDATA, 'Local', appDataDir);
    } else if (process.env.USERPROFILE) {
      localDir = path.join(process.env.USERPROFILE, 'AppData', 'Local', appDataDir);
    }

    if (localDir) {
      return [globalDir, localDir];
    } else {
      return [globalDir];
    }
  },
  darwin: () => {
    const home = os.homedir();
    const userFolders = home
      ? [path.join(home, '/Library/Fonts')]
      : [];
    return [
      ...userFolders,
      '/Library/Fonts',
      '/Network/Library/Fonts',
      '/System/Library/Fonts',
      '/System Folder/Fonts'
    ];
  },
  linux: () => {
    const home = os.homedir();
    const userFolders = home
      ? [
        path.join(home, '.fonts'),
        path.join(home, '.local/share/fonts')
      ]
      : [];
    return [
      // TODO: use fontconfig to find the folder locations
      '/usr/share/fonts',
      '/usr/local/share/fonts',
      ...userFolders
    ];
  },
  android: () => {
    const home = os.homedir();
    const userFolders = home
      ? [
        path.join(home, '.fonts'),
        path.join(home, '.local/share/fonts')
      ]
      : [];
    return [
      // TODO: use fontconfig to find the folder locations
      '/system/fonts',
      ...userFolders
    ];
  }
};

export interface Options {
  /**
   * Additional folders to search for fonts. Default: []
   */
  additionalFolders?: string[];

  /**
   * File extensions to search for. Default: ['ttf', 'otf', 'ttc', 'woff', 'woff2']
   */
  extensions?: string[];
}

/**
 * List absolute paths to all installed system fonts present.
 *
 * @param options Configuration options
 */
function getSystemFonts(options?: Options): Promise<string[]> {
  const opts: Required<Options> = {
    extensions: ['ttf', 'otf', 'ttc', 'woff', 'woff2'],
    additionalFolders: [],
    ...options
  };

  const platform = os.platform();

  const getDirs = directories[platform];
  if (!getDirs) {
    throw new Error(`Unsupported platform: ${platform}`);
  }

  const dirs = getDirs();

  return recursiveWalk([...dirs, ...opts.additionalFolders], opts.extensions);
}

export default getSystemFonts;
