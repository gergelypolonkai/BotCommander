# Copyright 1999-2007 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit eutils versionator gnome2

DESCRIPTION="Eggdrop IRC bot maintanence program"
HOMEPAGE="http://www.botcommander.hu/"
SRC_URI="http://www.botcommander.hu/dl/${P}.tar.gz"
LICENSE="GPL-2"
SLOT="0"
KEYWORDS="x86 amd64"
IUSE="-debug"
DEPEND=""
RDEPEND=">=dev-libs/glib-2.6.0
	>=x11-libs/gtk+-2.10.0"

src_compile() {
	econf \
		--prefix="${D}"/usr \
		--mandir="${D}"/usr/share/man \
		--infodir="${D}"/usr/share/info \
		--libdir="${D}"/usr/$(get_libdir) || die "econf failed"
	emake || die "emake failed"
}

src_install() {
	emake \
		prefix="${D}"/usr \
		mandir="${D}"/usr/share/man \
		infodir="${D}"/usr/share/info \
		libdir="${D}"/usr/$(get_libdir) \
		install || die "emake install failed"
}
