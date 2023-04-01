import math
from matplotlib.patches import Polygon
from matplotlib.collections import PatchCollection
from matplotlib.colors import to_rgba
import matplotlib.pyplot as plt
import numpy as np

def setup_plot_for_polygon(polygon,simple=False):
    if simple:
        _, ax = plt.subplots()
        ax.add_patch(Polygon(polygon.points, closed=True, alpha=.5, fc="gray"))
        # holes
        patches = []
        for h in polygon.holes:
            patches.append(Polygon(h, closed=True))
        patch_collection = PatchCollection(patches, fc="white")
        ax.add_collection(patch_collection)
        ax.axis("equal")
        ax.axis("off")
        plt.tight_layout()
    else:
        _, ax = plt.subplots()
        plt.title("Input polygon {}".format(polygon.name), fontsize=20)
        patches = []
        patches.append(Polygon(polygon.points, closed=True))
        for h in polygon.holes:
            patches.append(Polygon(h, closed=True))

        patch_collection = PatchCollection(patches, alpha=0.4, ec="red")
        ax.add_collection(patch_collection)
        ax.margins(.1)
        ax.axis("equal")
        ax.autoscale()

def setup_plot_for_cover(cover, simple=False):

    plot_input_polygon = False

    fig, axs = plt.subplots(nrows=1, ncols=(2 if plot_input_polygon else 1))
    if not simple:
        plt.title(f"plot of Cover({cover.name}) of Polygon({cover.polygon.name})", fontsize=20)

    # input polygon
    if plot_input_polygon:
        # polygon
        axs[0].add_patch(Polygon(cover.polygon.points, closed=True, alpha=.5, fc="gray"))# , ec="black", lw=2))
        # holes
        patches = []
        for h in cover.polygon.holes:
            patches.append(Polygon(h, closed=True))
        patch_collection = PatchCollection(patches, fc="white")#, ec=to_rgba("black", .5), lw=2)
        axs[0].add_collection(patch_collection)

    cover_faces = cover.get_number_faces()

    # cover
    def get_random_colors(n):
        vals = np.linspace(0,1,n)
        np.random.shuffle(vals)
        colors = plt.cm.colors.ListedColormap(plt.cm.jet(vals))
        return [plt.cm.colors.to_rgba(colors(i), alpha=0.3) for i in range(n)], [plt.cm.colors.to_rgba(colors(i), alpha=.6) for i in range(n)]

    colors1, colors1_noalpha = get_random_colors(len(cover_faces))
    #colors2 = ["black"]*len(cover.faces) #get_random_colors(len(cover.faces))
    colors2 = [plt.cm.colors.to_rgba("black", alpha=0.9) for i in range(len(cover_faces))]
    faces = []
    for i, f in enumerate(cover_faces):
        face = Polygon(f, closed=True, facecolor=colors1[i], ec=colors1_noalpha[i], lw=2)
        if plot_input_polygon: axs[1].add_patch(face)
        else: axs.add_patch(face)
        faces.append(face)

    if not simple:
        centroids = []
        for f in cover_faces: centroids.append((sum([e[0] for e in f])/len(f), sum([e[1] for e in f])/len(f)))
        if plot_input_polygon: axs[1].scatter([c[0] for c in centroids],[c[1] for c in centroids])
        else: axs.scatter([c[0] for c in centroids],[c[1] for c in centroids])

    if plot_input_polygon:
        for ax in axs:
            ax.axis("equal")
    else:
        axs.axis("equal")

    if simple:
        if plot_input_polygon:
            for ax in axs:
                ax.axis("off")
        else:
            axs.axis("off")
    else:
        if plot_input_polygon: 
            axs[1].margins(.1)
            axs[1].autoscale()
        else:
            axs.margins(.1)
            axs.autoscale()

    plt.tight_layout()

    if not simple:
        def on_click(event):
            min_d = math.inf
            idx = -1
            for i, centroid in enumerate(centroids):
                point = axs[1].transData.inverted().transform((event.x, event.y)) if plot_input_polygon else axs.transData.inverted().transform((event.x, event.y))
                d = math.dist(point, centroid)
                if d <= min_d:
                    min_d = d
                    idx = i
            if faces[idx].get_facecolor() == colors1[idx]:
                faces[idx].set_facecolor(colors2[idx])
            else:
                faces[idx].set_facecolor(colors1[idx])
            fig.canvas.draw()
            fig.canvas.flush_events()
        plt.connect('button_press_event', on_click)
