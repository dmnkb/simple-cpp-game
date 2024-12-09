/**
 * As taken from https://irrlicht.sourceforge.io/docu/classirr_1_1scene_1_1_i_scene_node.html
 *
 * A scene node is a node in the hierarchical scene graph. Every scene node may have children,
 * which are also scene nodes. Children move relative to their parent's position.
 * If the parent of a node is not visible, its children won't be visible either.
 * In this way, it is for example easily possible to attach a light to a moving car,
 * or to place a walking character on a moving platform on a moving ship.
 */

// TODO: