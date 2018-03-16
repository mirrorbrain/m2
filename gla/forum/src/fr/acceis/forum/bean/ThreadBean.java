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
import java.util.ArrayList;

/**
 * The Class ThreadBean.
 */
public class ThreadBean {

	/** The author. */
	private final String author;

	/** The created. */
	private final Date created;

	/** The id. */
	private final int id;

	/** The posts. */
	private final ArrayList<PostBean> posts = new ArrayList<>();

	/** The title. */
	private String title;

	/**
	 * Instantiates a new thread bean.
	 *
	 * @param id
	 *            the id
	 * @param title
	 *            the title
	 * @param author
	 *            the author
	 * @param created
	 *            the created
	 */
	public ThreadBean(final int id, final String title, final String author, final Date created) {
		super();
		this.id = id;
		this.title = title;
		this.author = author;
		this.created = created;
	}

	/**
	 * Adds the post.
	 *
	 * @param post
	 *            the post
	 */
	public void addPost(final PostBean post) {
		posts.add(post);
	}

	/**
	 * Gets the author.
	 *
	 * @return the author
	 */
	public String getAuthor() {
		return author;
	}

	/**
	 * Gets the created.
	 *
	 * @return the created
	 */
	public Date getCreated() {
		return created;
	}

	/**
	 * Gets the id.
	 *
	 * @return the id
	 */
	public int getId() {
		return id;
	}

	/**
	 * Gets the number of posts.
	 *
	 * @return the number of posts
	 */
	public int getNumberOfPosts() {
		return posts.size();
	}

	/**
	 * Gets the posts.
	 *
	 * @return the posts
	 */
	public ArrayList<PostBean> getPosts() {
		return (ArrayList<PostBean>) posts.clone();
	}

	/**
	 * Gets the title.
	 *
	 * @return the title
	 */
	public String getTitle() {
		return title;
	}

	/**
	 * Sets the title.
	 *
	 * @param title
	 *            the new title
	 */
	public void setTitle(final String title) {
		this.title = title;
	}
}
