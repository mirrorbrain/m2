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
package fr.acceis.forum.db;

import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;
import java.sql.Connection;
import java.sql.Date;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;

import fr.acceis.forum.bean.PostBean;
import fr.acceis.forum.bean.ThreadBean;
import fr.acceis.forum.bean.UserBean;

/**
 * The Class Utils.
 */
public class Utils {

	/** The Constant SALT_SIZE. */
	private static final int SALT_SIZE = 64;

	/**
	 * Adds the user.
	 *
	 * @param login
	 *            the login
	 * @param password
	 *            the password
	 * @param mail
	 *            the mail
	 * @return the string
	 */
	public static boolean addUser(final String login, final String password, final String mail) {
		try {
			final Connection c = Connexion.connect();
			PreparedStatement s = c.prepareStatement("select max(id) from users");
			long id = 0;
			if (s.execute()) {
				final ResultSet r = s.getResultSet();
				if (r.next())
					id = r.getLong(1) + 1;
			} else
				throw new SQLException("could not get the current max id - users");

			final String q = "insert into users values(?,?,?,?,?,?,?)";
			s = c.prepareStatement(q);

			final byte[] salt = generateSalt();
			final byte[] hash = calculateHash(password, salt);

			s.setLong(1, id);
			s.setString(2, login);
			s.setBytes(3, hash);
			s.setString(4, mail);
			s.setDate(5, new Date(new java.util.Date().getTime()));
			s.setLong(6, 0);
			s.setBytes(7, salt);
			s.execute();

			s.close();
			c.close();
			return true;
		} catch (final SQLException | InstantiationException | IllegalAccessException | ClassNotFoundException
				| NoSuchAlgorithmException | UnsupportedEncodingException e) {
			System.err.println("Error on user add :\t" + e.getMessage());
			e.printStackTrace();
		}
		return false;
	}

	/**
	 * Calculate hash.
	 *
	 * @param password
	 *            the password
	 * @param salt
	 *            the salt
	 * @return the byte[]
	 * @throws NoSuchAlgorithmException
	 *             the no such algorithm exception
	 * @throws UnsupportedEncodingException
	 *             the unsupported encoding exception
	 */
	private static byte[] calculateHash(final String password, final byte[] salt)
			throws NoSuchAlgorithmException, UnsupportedEncodingException {
		final MessageDigest md = MessageDigest.getInstance("SHA-512");
		md.reset();
		md.update(salt);
		byte[] hash = md.digest(password.getBytes("UTF-8"));

		for (int i = 0; i < 1000000; i++) {
			md.reset();
			hash = md.digest(hash);
		}

		return hash;
	}

	/**
	 * Compare passwords.
	 *
	 * @param hash
	 *            the stored password
	 * @param salt
	 *            the salt
	 * @param passwordForm
	 *            the given password
	 * @return true, if successful
	 */
	private static boolean comparePasswords(final byte[] hash, final byte[] salt, final String passwordForm) {
		try {
			final byte[] comparisonHash = calculateHash(passwordForm, salt);

			int diff = hash.length ^ comparisonHash.length;
			for (int i = 0; i < hash.length && i < comparisonHash.length; i++)
				diff |= hash[i] ^ comparisonHash[i];

			return diff == 0;
		} catch (NoSuchAlgorithmException | UnsupportedEncodingException e) {
			System.err.println("Error while comparing passwords: " + e.getMessage());
			e.printStackTrace();
		}
		return false;
	}

	/**
	 * Connect user.
	 *
	 * @param login
	 *            the login
	 * @param password
	 *            the password
	 * @return true, if successful
	 */
	public static boolean connectUser(final String login, final String password) {
		try {
			final UserBean u = getUser(login);

			if (!comparePasswords(u.getPassword(), u.getSalt(), password))
				throw new SQLException("wrong password");

			return true;
		} catch (final SQLException | InstantiationException | IllegalAccessException | ClassNotFoundException e) {
			System.err.println("Error while connecting the user " + login + ": " + e.getMessage());
			e.printStackTrace();
		}
		return false;
	}

	/**
	 * Search for the given login in the database.
	 *
	 * @param login
	 *            the login
	 * @return true, if it exists
	 */
	public static boolean exists(final String login) {
		Connection c;
		try {
			c = Connexion.connect();
			final PreparedStatement s = c.prepareStatement("select login from users where login=?");

			s.setString(1, login);
			final ResultSet r = s.executeQuery();

			if (r.next())
				return r.getString(1) != null;
		} catch (InstantiationException | IllegalAccessException | ClassNotFoundException | SQLException e) {
			System.err.println("Error while searching users named " + login + ": " + e.getMessage());
			e.printStackTrace();
		}

		return false;
	}

	/**
	 * Generate salt.
	 *
	 * @return the byte[]
	 */
	private static byte[] generateSalt() {
		final SecureRandom random = new SecureRandom();
		final byte[] salt = new byte[SALT_SIZE];
		random.nextBytes(salt);

		return salt;
	}

	/**
	 * Gets the number of posts published by the user.
	 *
	 * @param login
	 *            the login
	 * @return the number of posts
	 */
	public static long getNumberOfPosts(final String login) {
		try {
			final Connection c = Connexion.connect();
			final PreparedStatement s = c.prepareStatement("select posts from users where login=?");

			s.setString(1, login);
			final ResultSet r = s.executeQuery();

			if (r.next())
				return r.getLong("posts");
		} catch (InstantiationException | IllegalAccessException | ClassNotFoundException | SQLException e) {
			System.err.println(
					"Error while retrieving the number of posts for the user " + login + ": " + e.getMessage());
			e.printStackTrace();
		}
		return 0;
	}

	/**
	 * Gets the threads.
	 *
	 * @return the threads
	 */
	public static ArrayList<ThreadBean> getThreads() {
		final ArrayList<ThreadBean> ret = new ArrayList<>();
		try {
			final Connection c = Connexion.connect();
			final PreparedStatement s = c.prepareStatement("select * from threads order by id asc");
			final ResultSet r = s.executeQuery();

			while (r.next()) {
				final ThreadBean t = new ThreadBean(r.getInt("id"), r.getString("title"), r.getString("author"),
						r.getDate("created"));
				final PreparedStatement sp = c.prepareStatement("select * from posts where parent=? order by id asc");
				sp.setInt(1, t.getId());
				final ResultSet rp = sp.executeQuery();

				while (rp.next())
					t.addPost(new PostBean(rp.getDate("posted"), rp.getDate("edited"), rp.getString("title"),
							rp.getString("post"), rp.getString("author"), rp.getInt("id"), rp.getInt("parent")));

				ret.add(t);

				rp.close();
				sp.close();
			}

			r.close();
			s.close();
			c.close();
		} catch (InstantiationException | IllegalAccessException | ClassNotFoundException | SQLException e) {
			System.err.println("Error while retrieving the threads: " + e.getMessage());
			e.printStackTrace();
		}
		return ret;
	}

	/**
	 * Gets the user.
	 *
	 * @param login
	 *            the login
	 * @return the user
	 * @throws SQLException
	 *             the SQL exception
	 * @throws InstantiationException
	 *             the instantiation exception
	 * @throws IllegalAccessException
	 *             the illegal access exception
	 * @throws ClassNotFoundException
	 *             the class not found exception
	 */
	private static UserBean getUser(final String login)
			throws SQLException, InstantiationException, IllegalAccessException, ClassNotFoundException {
		final Connection c = Connexion.connect();
		final PreparedStatement s = c
				.prepareStatement("select login, password, mail, joined, salt from users where login=?");

		s.setString(1, login);
		final ResultSet r = s.executeQuery();
		UserBean ret = null;

		if (r.next())
			ret = new UserBean(r.getString("login"), r.getBytes("password"), r.getString("mail"), r.getDate("joined"),
					r.getBytes("salt"));
		else
			throw new SQLException("invalid user");

		r.close();
		s.close();
		c.close();
		return ret;
	}

	/**
	 * New post.
	 *
	 * @param parent
	 *            the parent
	 * @param title
	 *            the title
	 * @param post
	 *            the post
	 * @param author
	 *            the author
	 */
	public static void newPost(final long parent, final String title, final String post, final String author) {
		try {
			final Connection c = Connexion.connect();
			// new post
			PreparedStatement s = c.prepareStatement("select max(id) from posts");
			ResultSet r;

			long id = 0;
			if (s.execute()) {
				r = s.getResultSet();
				if (r.next())
					id = r.getLong(1) + 1;
			} else
				throw new SQLException("could not get the current max id - posts");

			s = c.prepareStatement("insert into posts values(?,?,?,?,?,?,?)");

			s.setLong(1, id);
			s.setLong(2, parent);
			s.setString(3, title);
			s.setString(4, author);
			s.setString(5, post);
			s.setDate(6, new Date(new java.util.Date().getTime()));
			s.setDate(7, new Date(new java.util.Date().getTime()));
			s.execute();

			s = c.prepareStatement("update users set posts = posts + 1 where login = ?");

			s.setString(1, author);
			s.execute();

			r.close();
			s.close();
			c.close();
		} catch (InstantiationException | IllegalAccessException | ClassNotFoundException | SQLException e) {
			System.err.println("Error while creating the post: " + e.getMessage());
			e.printStackTrace();
		}
	}

	/**
	 * New thread.
	 *
	 * @param title
	 *            the title
	 * @param post
	 *            the post
	 * @param author
	 *            the author
	 * @return the new thread's id
	 */
	public static long newThread(final String title, final String post, final String author) {
		long ret = 0;

		try {
			final Connection c = Connexion.connect();

			// new thread
			PreparedStatement s = c.prepareStatement("select max(id) from threads");
			ResultSet r;

			if (s.execute()) {
				r = s.getResultSet();
				if (r.next())
					ret = r.getLong(1) + 1;
			} else
				throw new SQLException("could not get the current max id - threads");

			s = c.prepareStatement("insert into threads values(?,?,?,?)");

			s.setLong(1, ret);
			s.setString(2, title);
			s.setString(3, author);
			s.setDate(4, new Date(new java.util.Date().getTime()));
			s.execute();

			// new post
			newPost(ret, title, post, author);

			r.close();
			s.close();
			c.close();
		} catch (InstantiationException | IllegalAccessException | ClassNotFoundException | SQLException e) {
			System.err.println("Error while creating the thread: " + e.getMessage());
			e.printStackTrace();
		}

		return ret;
	}
}
