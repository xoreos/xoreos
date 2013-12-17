dnl @synopsis AX_CHECK_OGRE(version, action-if, action-if-not)
dnl
dnl @summary check for OGRE of sufficient version
dnl
dnl Defines OGRE_LIBS, OGRE_CFLAGS and OGRE_PLUGINDIR.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2013-12-14
dnl @license AllPermissive

AC_DEFUN([AX_CHECK_OGRE], [
	PKG_CHECK_MODULES(OGRE, [OGRE >= $1], $2, $3)

	OGRE_PLUGINDIR=`$PKG_CONFIG --variable=plugindir OGRE`

	AC_SUBST(OGRE_CFLAGS)
	AC_SUBST(OGRE_LIBS)
	AC_SUBST(OGRE_PLUGINDIR)
])

dnl @synopsis AX_CHECK_OGREOVERLAY(version, action-if, action-if-not)
dnl
dnl @summary check for OGRE-Overlay of sufficient version
dnl
dnl Defines OGREOVERLAY_LIBS and OGREOVERLAY_CFLAGS.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2013-12-14
dnl @license AllPermissive

AC_DEFUN([AX_CHECK_OGREOVERLAY], [
	PKG_CHECK_MODULES(OGREOVERLAY, [OGRE-Overlay >= $1], $2, $3)

	AC_SUBST(OGREOVERLAY_CFLAGS)
	AC_SUBST(OGREOVERLAY_LIBS)
])
