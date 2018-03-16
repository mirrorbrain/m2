/*
 * The GNU General Public License does not permit incorporating your program
 * into proprietary programs. If your program is a subroutine library, you may
 * consider it more useful to permit linking proprietary applications with the
 * library. If this is what you want to do, use the GNU Lesser General Public
 * License instead of this License. But first, please read
 * <http://www.gnu.org/philosophy/why-not-lgpl.html>.
 * ----------------------------------------------------------------------------
 * This source file is part of a university project and belongs to its creator
 * and the university it was made for.
 * @author Maël Nogues mael.nogues@outlook.com
 */
package fr.acceis.forum.bean;

import java.sql.Date;
import java.util.Arrays;

/**
 * The Class UserBean.
 */
public class UserBean {

	/** The joined. */
	private final Date joined;

	/** The mail. */
	private final String login;

	/** The mail. */
	private final String mail;

	/** The password. */
	private final byte[] password;

	/** The salt. */
	private final byte[] salt;

	/**
	 * Instantiates a new user bean.
	 *
	 * @param login
	 *            the login
	 * @param hash
	 *            the hash
	 * @param mail
	 *            the mail
	 * @param joined
	 *            the joined
	 * @param salt
	 *            the salt
	 */
	public UserBean(final String login, final byte[] hash, final String mail, final Date joined, final byte[] salt) {
		super();
		this.login = login;
		password = hash;
		this.mail = mail;
		this.joined = joined;
		this.salt = salt;
	}

	/**
	 * Gets the joined.
	 *
	 * @return the joined
	 */
	public Date getJoined() {
		return joined;
	}

	/**
	 * Gets the login.
	 *
	 * @return the login
	 */
	public String getLogin() {
		return login;
	}

	/**
	 * Gets the mail.
	 *
	 * @return the mail
	 */
	public String getMail() {
		return mail;
	}

	/**
	 * Gets the password.
	 *
	 * @return the password
	 */
	public byte[] getPassword() {
		return Arrays.copyOf(password, password.length);
	}

	/**
	 * Gets the salt.
	 *
	 * @return the salt
	 */
	public byte[] getSalt() {
		return Arrays.copyOf(salt, salt.length);
	}
}
